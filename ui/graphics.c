/*
 * Copyright (C) 2007 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

//#include "graphics.h"



#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*#include <memory>
#include <string>
#include <vector>
*/
#include "font_10x18.h"
//#include "graphics_adf.h"
#ifdef DRECOVERY_RGBX
#include "graphics_drm.h"
#endif
//#include "graphics_fbdev.h"
#include "minui/minui.h"



static GRFont* gr_font = NULL;
//static MinuiBackend* gr_backend = nullptr;

static int overscan_percent = OVERSCAN_PERCENT;
static int overscan_offset_x = 0;
static int overscan_offset_y = 0;

static uint32_t gr_current = ~0;
static /*constexpr*/ uint32_t alpha_mask = 0xff000000;

static GRSurface* gr_draw = NULL;
static GRRotation rotation = ROTATION_NONE;

static bool outside(int x, int y) {
  return x < 0 || x >= (rotation % 2 ? gr_draw->height : gr_draw->width) || y < 0 ||
         y >= (rotation % 2 ? gr_draw->width : gr_draw->height);
}

const GRFont* gr_sys_font() {
  return gr_font;
}

int gr_measure(const GRFont* font, const char* s) {
  return font->char_width * strlen(s);
}

void gr_font_size(const GRFont* font, int* x, int* y) {
  *x = font->char_width;
  *y = font->char_height;
}

// Blends gr_current onto pix value, assumes alpha as most significant byte.
static inline uint32_t pixel_blend(uint8_t alpha, uint32_t pix) {
  if (alpha == 255) return gr_current;
  if (alpha == 0) return pix;
  uint32_t pix_r = pix & 0xff;
  uint32_t pix_g = pix & 0xff00;
  uint32_t pix_b = pix & 0xff0000;
  uint32_t cur_r = gr_current & 0xff;
  uint32_t cur_g = gr_current & 0xff00;
  uint32_t cur_b = gr_current & 0xff0000;

  uint32_t out_r = (pix_r * (255 - alpha) + cur_r * alpha) / 255;
  uint32_t out_g = (pix_g * (255 - alpha) + cur_g * alpha) / 255;
  uint32_t out_b = (pix_b * (255 - alpha) + cur_b * alpha) / 255;

  return (out_r & 0xff) | (out_g & 0xff00) | (out_b & 0xff0000) | (gr_current & 0xff000000);
}

// increments pixel pointer right, with current rotation.
static void incr_x(uint32_t** p, int row_pixels) {
  if (rotation % 2) {
    *p = *p + (rotation == 1 ? 1 : -1) * row_pixels;
  } else {
    *p = *p + (rotation ? -1 : 1);
  }
}

// increments pixel pointer down, with current rotation.
static void incr_y(uint32_t** p, int row_pixels) {
  if (rotation % 2) {
    *p = *p + (rotation == 1 ? -1 : 1);
  } else {
    *p = *p + (rotation ? -1 : 1) * row_pixels;
  }
}

// returns pixel pointer at given coordinates with rotation adjustment.
static uint32_t* pixel_at(GRSurface* surf, int x, int y, int row_pixels) {
  switch (rotation) {
    case ROTATION_NONE:
      return (uint32_t*)/*reinterpret_cast<uint32_t*>*/(surf->data) + y * row_pixels + x;
    case ROTATION_RIGHT:
      return (uint32_t*)/*reinterpret_cast<uint32_t*>*/(surf->data) + x * row_pixels + (surf->width - y);
    case ROTATION_DOWN:
      return (uint32_t*)/*reinterpret_cast<uint32_t*>*/(surf->data) + (surf->height - 1 - y) * row_pixels +
             (surf->width - 1 - x);
    case ROTATION_LEFT:
      return (uint32_t*)/*reinterpret_cast<uint32_t*>*/(surf->data) + (surf->height - 1 - x) * row_pixels + y;
    default:
      printf("invalid rotation %d", rotation);
  }
  return nullptr;
}

static void text_blend(uint8_t* src_p, int src_row_bytes, uint32_t* dst_p, int dst_row_pixels,
                       int width, int height) {
  uint8_t alpha_current =(uint8_t)/* static_cast<uint8_t>*/((alpha_mask & gr_current) >> 24);
  for (int j = 0; j < height; ++j) {
    uint8_t* sx = src_p;
    uint32_t* px = dst_p;
    for (int i = 0; i < width; ++i, incr_x(&px, dst_row_pixels)) {
      uint8_t a = *sx++;
      if (alpha_current < 255) a = (/*static_cast<uint32_t>*/(uint32_t)(a) * alpha_current) / 255;
      *px = pixel_blend(a, *px);
    }
    src_p += src_row_bytes;
    incr_y(&dst_p, dst_row_pixels);
  }
}

void gr_text(const GRFont* font, int x, int y, const char* s, bool bold) {
  if (!font || !font->texture || (gr_current & alpha_mask) == 0) return;

  if (font->texture->pixel_bytes != 1) {
    printf("gr_text: font has wrong format\n");
    return;
  }

  bold = bold && (font->texture->height != font->char_height);

  x += overscan_offset_x;
  y += overscan_offset_y;

  unsigned char ch;
  while ((ch = *s++)) {
    if (outside(x, y) || outside(x + font->char_width - 1, y + font->char_height - 1)) break;

    if (ch < ' ' || ch > '~') {
      ch = '?';
    }

    int row_pixels = gr_draw->row_bytes / gr_draw->pixel_bytes;
    uint8_t* src_p = font->texture->data + ((ch - ' ') * font->char_width) +
                     (bold ? font->char_height * font->texture->row_bytes : 0);
    uint32_t* dst_p = pixel_at(gr_draw, x, y, row_pixels);

    text_blend(src_p, font->texture->row_bytes, dst_p, row_pixels, font->char_width,
               font->char_height);

    x += font->char_width;
  }
}

void gr_texticon(int x, int y, GRSurface* icon) {
  if (icon == NULL) return;

  if (icon->pixel_bytes != 1) {
    printf("gr_texticon: source has wrong format\n");
    return;
  }

  x += overscan_offset_x;
  y += overscan_offset_y;

  if (outside(x, y) || outside(x + icon->width - 1, y + icon->height - 1)) return;

  int row_pixels = gr_draw->row_bytes / gr_draw->pixel_bytes;
  uint8_t* src_p = icon->data;
  uint32_t* dst_p = pixel_at(gr_draw, x, y, row_pixels);

  text_blend(src_p, icon->row_bytes, dst_p, row_pixels, icon->width, icon->height);
}

void gr_color(unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
  uint32_t r32 = b, g32 = g, b32 = r, a32 = a;
#if defined(RECOVERY_ABGR) || defined(RECOVERY_BGRA)
  gr_current = (a32 << 24) | (r32 << 16) | (g32 << 8) | b32;
#else
  gr_current = (a32 << 24) | (b32 << 16) | (g32 << 8) | r32;
#endif
}

void gr_clear() {
  if ((gr_current & 0xff) == ((gr_current >> 8) & 0xff) &&
      (gr_current & 0xff) == ((gr_current >> 16) & 0xff) &&
      (gr_current & 0xff) == ((gr_current >> 24) & 0xff) &&
      gr_draw->row_bytes == gr_draw->width * gr_draw->pixel_bytes) {
    memset(gr_draw->data, gr_current & 0xff, gr_draw->height * gr_draw->row_bytes);
  } else {
    uint32_t* px = (uint32_t*)/*reinterpret_cast<uint32_t*>*/(gr_draw->data);
    int row_diff = gr_draw->row_bytes / gr_draw->pixel_bytes - gr_draw->width;
    for (int y = 0; y < gr_draw->height; ++y) {
      for (int x = 0; x < gr_draw->width; ++x) {
        *px++ = gr_current;
      }
      px += row_diff;
    }
  }
}

void gr_fill(int x1, int y1, int x2, int y2) {
  x1 += overscan_offset_x;
  y1 += overscan_offset_y;

  x2 += overscan_offset_x;
  y2 += overscan_offset_y;

  if (outside(x1, y1) || outside(x2 - 1, y2 - 1)) return;

  int row_pixels = gr_draw->row_bytes / gr_draw->pixel_bytes;
  uint32_t* p = pixel_at(gr_draw, x1, y1, row_pixels);
  uint8_t alpha = (uint8_t)/*static_cast<uint8_t>*/(((gr_current & alpha_mask) >> 24));
  if (alpha > 0) {
    for (int y = y1; y < y2; ++y) {
      uint32_t* px = p;
      for (int x = x1; x < x2; ++x) {
        *px = pixel_blend(alpha, *px);
        incr_x(&px, row_pixels);
      }
      incr_y(&p, row_pixels);
    }
  }
}

void gr_blit(GRSurface* source, int sx, int sy, int w, int h, int dx, int dy) {
  if (source == NULL) return;

  if (gr_draw->pixel_bytes != source->pixel_bytes) {
    printf("gr_blit: source has wrong format\n");
    return;
  }

  dx += overscan_offset_x;
  dy += overscan_offset_y;

  if (outside(dx, dy) || outside(dx + w - 1, dy + h - 1)) return;

  if (rotation) {
    int src_row_pixels = source->row_bytes / source->pixel_bytes;
    int row_pixels = gr_draw->row_bytes / gr_draw->pixel_bytes;
    uint32_t* src_py = (uint32_t*)/*reinterpret_cast<uint32_t*>*/(source->data) + sy * source->row_bytes / 4 + sx;
    uint32_t* dst_py = pixel_at(gr_draw, dx, dy, row_pixels);

    for (int y = 0; y < h; y += 1) {
      uint32_t* src_px = src_py;
      uint32_t* dst_px = dst_py;
      for (int x = 0; x < w; x += 1) {
        *dst_px = *src_px++;
        incr_x(&dst_px, row_pixels);
      }
      src_py += src_row_pixels;
      incr_y(&dst_py, row_pixels);
    }
  } else {
    unsigned char* src_p = source->data + sy * source->row_bytes + sx * source->pixel_bytes;
    unsigned char* dst_p = gr_draw->data + dy * gr_draw->row_bytes + dx * gr_draw->pixel_bytes;

    int i;
    for (i = 0; i < h; ++i) {
      memcpy(dst_p, src_p, w * source->pixel_bytes);
      src_p += source->row_bytes;
      dst_p += gr_draw->row_bytes;
    }
  }
}

unsigned int gr_get_width(GRSurface* surface) {
  if (surface == NULL) {
    return 0;
  }
  return surface->width;
}

unsigned int gr_get_height(GRSurface* surface) {
  if (surface == NULL) {
    return 0;
  }
  return surface->height;
}

int gr_init_font(const char* name, GRFont** dest) {
  GRFont* font = (GRFont*)/*static_cast<GRFont*>*/(calloc(1, sizeof(*gr_font)));
  if (font == nullptr) {
    return -1;
  }

  int res = res_create_alpha_surface(name, &(font->texture));
  printf("gr_init_font %d\n",res);
  if (res < 0) {
  	if(font->texture)res_free_surface(font->texture);
    free(font);
    return res;
  }

  // The font image should be a 96x2 array of character images.  The
  // columns are the printable ASCII characters 0x20 - 0x7f.  The
  // top row is regular text; the bottom row is bold.
  font->char_width = font->texture->width / 96;
  font->char_height = font->texture->height / 2;

  *dest = font;

  return 0;
}

static void _gr_init_font(void) {
	
  int res;
  //if(is_hdmi())
  //	;//res= gr_init_font("/tmp/18x32.png", &gr_font);
  //else
  //	res= gr_init_font("/tmp/font.png", &gr_font);
  res= gr_init_font("/usr/lib/libretro/games/font.png", &gr_font);
  if (res == 0) {
    return;
  }

  printf("failed to read font: res=%d\n", res);

  // fall back to the compiled-in font.
  gr_font = (GRFont*)/*static_cast<GRFont*>*/(calloc(1, sizeof(*gr_font)));
  gr_font->texture = (GRSurface*)/*static_cast<GRSurface*>*/(malloc(sizeof(*gr_font->texture)));
  gr_font->texture->width = font.width;
  gr_font->texture->height = font.height;
  gr_font->texture->row_bytes = font.width;
  gr_font->texture->pixel_bytes = 1;

  unsigned char* bits = (unsigned char*)(malloc(font.width * font.height));
  gr_font->texture->data = bits;

  unsigned char data;
  unsigned char* in = font.rundata;
  while ((data = *in++)) {
    memset(bits, (data & 0x80) ? 255 : 0, data & 0x7f);
    bits += (data & 0x7f);
  }

  gr_font->char_width = font.char_width;
  gr_font->char_height = font.char_height;
}

void gr_flip() {
 // gr_draw = gr_backend->Flip();
}

int gr_init(int _width,int _height,int _linelength,int _bitsperpixel, unsigned char* _bits) 
{
  _gr_init_font();
#if 0
#ifdef DRECOVERY_RGBX
  auto backend = std::unique_ptr<MinuiBackend>{ std::make_unique<MinuiBackendDrm>() };
  gr_draw = backend->Init();
#else
  auto backend = std::unique_ptr<MinuiBackend>{ std::make_unique<MinuiBackendFbdev>() };
  gr_draw = backend->Init();
#endif

  if (!gr_draw) {
    return -1;
  }

  gr_backend = backend.release();

  overscan_offset_x = gr_draw->width * overscan_percent / 100;
  overscan_offset_y = gr_draw->height * overscan_percent / 100;

  gr_flip();
  gr_flip();
#else
	gr_draw = (GRSurface*)(malloc(sizeof(GRSurface)));
	gr_draw->width = _width;
	gr_draw->height = _height;
	gr_draw->row_bytes = _linelength;
	gr_draw->pixel_bytes = _bitsperpixel / 8;
	gr_draw->data = (uint8_t*)(_bits);

   

#endif
  gr_rotate(DEFAULT_ROTATION);

  if (gr_draw->pixel_bytes != 4) {
    printf("gr_init: Only 4-byte pixel formats supported\n");
  }

  return 0;
}

void gr_exit() {
 // delete gr_backend;
	 if(gr_draw)free(gr_draw);
	 gr_draw=NULL;
	 if(gr_font)
	 {
	 	if(gr_font->texture)
			res_free_surface(gr_font->texture);
		free(gr_font);
	 }
}

int gr_fb_width() {
  return rotation % 2 ? gr_draw->height - 2 * overscan_offset_y
                      : gr_draw->width - 2 * overscan_offset_x;
}

int gr_fb_height() {
  return rotation % 2 ? gr_draw->width - 2 * overscan_offset_x
                      : gr_draw->height - 2 * overscan_offset_y;
}

void gr_fb_blank(bool blank) {
 // gr_backend->Blank(blank);
}

void gr_rotate(GRRotation rot) {
  rotation = rot;
}
