
#include <linux/fb.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <asm/types.h>

#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fbviewer.h"

#ifdef VID_DRM
#include "drm_display.h"
static int drm_get_preferred_mode(int *width, int *height)
{
    const char *path;
    char buf[256] = "\0";
    int fd, w, h;

#define DRM_MODE_CFG_PATH_ENV	"DRM_CONNECTOR_CFG_PATH"
#define DRM_MODE_CFG_PATH_DEFAULT	"/tmp/drm_mode.cfg"
    path = getenv(DRM_MODE_CFG_PATH_ENV);
    if (!path)
        path = DRM_MODE_CFG_PATH_DEFAULT;

    fd = open(path, O_RDONLY);
    read(fd, buf, sizeof(buf));
    close(fd);

    if (!buf[0])
        return -1;

    if (2 != sscanf(buf, "%dx%d", &w, &h))
        return -1;

    *width = w;
    *height = h;

    return 0;
}

#endif

#define DEFAULT_FRAMEBUFFER "/dev/fb0"
#if VIDEODISP

unsigned int fbAddr[3];
videoParam vparam;
renderBuf rBuf;
#endif

static int double_buffered;

int openFB(const char *name);
void closeFB(int fh);
void getVarScreenInfo(int fh, struct fb_var_screeninfo *var);
void setVarScreenInfo(int fh, struct fb_var_screeninfo *var);
void getFixScreenInfo(int fh, struct fb_fix_screeninfo *fix);

static int set_displayed_framebuffer(struct framebuffer *fb, unsigned int n)
{
	if (n > 1)
		return -1;
	//fb->var.yres_virtual = fb->var.yres * 2;
	fb->var.yoffset = n * fb->var.yres;
	if (ioctl(fb->fbh, FBIOPAN_DISPLAY, &fb->var) < 0) {
		fprintf(stderr, "active fb swap failed\n");
	}
	fb->displayed_buffer = n;
	return 0;
}

int fb_init(struct framebuffer *fb)
{
#if VIDEODISP
	VoutRect rect;
	int fb_w=800,fb_h=480;
	fb->mDispOutPort = CreateVideoOutport(0);
	if(fb->mDispOutPort == NULL){
		printf("CreateVideoOutport ERR");
		return -1;
	}
	rect.x = 0;
	rect.y = 0;
	rect.width = fb_w;//pic width
	rect.height = fb_h;//pic height
	fb->fb_tmp = (unsigned char *)malloc( 1024*768/*rect.width * rect.height */* 4);
	fb->mDispOutPort->init(fb->mDispOutPort, 0/*clear screen*/, 0, &rect);
	rect.x = 0;
	rect.y = 0;
	rect.width = fb->mDispOutPort->getScreenWidth(fb->mDispOutPort);
	rect.height = fb->mDispOutPort->getScreenHeight(fb->mDispOutPort);
	fb->screen_x = rect.width;
	fb->screen_y = rect.height;
	fb->cpp = 3;
	fb->x_stride = rect.width;
	fb->fix.line_length = rect.width*4;
	fb->var.bits_per_pixel = 32;
	fb->mDispOutPort->setRect(fb->mDispOutPort,&rect);
	fb->pMemops = GetMemAdapterOpsS();
    SunxiMemOpen(fb->pMemops);
	fb->pVirBuf = (char*)SunxiMemPalloc(fb->pMemops, fb_w * fb_h/*pic width x height*/ * 4 * 1/*buffer_num*/);
	fb->fb_base = (char*)SunxiMemGetPhysicAddressCpu(fb->pMemops, fb->pVirBuf);
	
	vparam.srcInfo.format = VIDEO_PIXEL_FORMAT_ARGB;
	vparam.srcInfo.crop_x = 0;
	vparam.srcInfo.crop_y = 0;
	vparam.srcInfo.crop_w = fb_w;
	vparam.srcInfo.crop_h = fb_h;

	vparam.srcInfo.w = fb_w;
	vparam.srcInfo.h = fb_h;
	vparam.srcInfo.color_space = VIDEO_BT601;
	 fbAddr[0] = (unsigned long)fb->fb_base;
     rBuf.y_phaddr = fbAddr[0];
	 rBuf.isExtPhy = VIDEO_USE_EXTERN_ION_BUF;
	fb->mDispOutPort->queueToDisplay(fb->mDispOutPort, vparam.srcInfo.w*vparam.srcInfo.h*4, &vparam, &rBuf);
	fb->mDispOutPort->SetZorder(fb->mDispOutPort,VIDEO_ZORDER_MIDDLE );//VIDEO_ZORDER_BOTTOM);// VIDEO_ZORDER_MIDDLE);
	fb->mDispOutPort->setEnable(fb->mDispOutPort, 1);
#elif VID_DRM
	int fb_w=480,fb_h=272;
	fb->ishdmi =0;
	drm_get_preferred_mode(&fb_w, &fb_h);
	if(fb_h> 272)
	{
		fb_h = 480;
		fb_w = 960;
		fb->ishdmi =1;
	}
	drm_init(1, 32, fb_w, fb_h);
	getdrmdispinfo(&fb_w,&fb_h);
	//printf("%d x %d \n",fb_w,fb_h);
	fb->fb_base = getdrmdispbuff();
	fb->fb_tmp = (unsigned char *)malloc( fb_w*fb_h*4);
	fb->screen_x = fb_w;
	fb->screen_y = fb_h;
	fb->var.xres =fb->screen_x;
	fb->var.yres=fb->screen_y;
	fb->cpp = 3;
	fb->x_stride = fb_w;
	fb->fix.line_length = fb_w*4;
	fb->var.bits_per_pixel = 32;
	//memset(fb->fb_tmp, 0xfafafafa, fb_w*fb_h*2);
	setdrmdisp(getdrmdisp());

#else
	/* get the framebuffer device handle */
	fb->fbh= openFB(NULL);
	if(fb->fbh == -1)
		return -1;

	/* read current video mode */
	getVarScreenInfo(fb->fbh, &fb->var);
	getFixScreenInfo(fb->fbh, &fb->fix);

	printf("fb0 reports (possibly inaccurate):\n"
           "  vi.bits_per_pixel = %d\n"
           "  vi.red.offset   = %3d   .length = %3d\n"
           "  vi.green.offset = %3d   .length = %3d\n"
           "  vi.blue.offset  = %3d   .length = %3d\n"
		"yres=%u\n"
		"yres_virtual=%u\n"
		"yoffset=%u\n",
           fb->var.bits_per_pixel,
           fb->var.red.offset, fb->var.red.length,
           fb->var.green.offset, fb->var.green.length,
           fb->var.blue.offset, fb->var.blue.length,
	   fb->var.yres, fb->var.yres_virtual, fb->var.yoffset);

	fb->screen_x = fb->var.xres;
	fb->screen_y = fb->var.yres;

	fb->x_stride = (fb->fix.line_length * 8) / fb->var.bits_per_pixel;

	switch (fb->var.bits_per_pixel) {
		case 8: fb->cpp = 1;break;
		case 16: fb->cpp = 2;break;
		case 24:
		case 32: fb->cpp = 4;break;
	}
	printf("singal=%u\nsmem_len=%u\n", fb->screen_x * fb->screen_y * fb->cpp, fb->fix.smem_len);
	fb->fb_base = (unsigned char*)mmap(NULL, fb->fix.smem_len, PROT_WRITE | PROT_READ, MAP_SHARED, fb->fbh, 0);

	fb->fb_tmp = (unsigned char *)malloc( fb->var.xres * fb->var.yres * fb->var.bits_per_pixel / 8);

	if (fb->var.yres * fb->fix.line_length * 2 <= fb->fix.smem_len) {
		double_buffered = 1;
		set_displayed_framebuffer(fb, 0);
	} else {
		double_buffered = 0;
		fb->displayed_buffer = 1;
		//fb->fb_tmp = (unsigned char *)malloc(fb->fix.smem_len);
	}

//printf("===framebuff===%x \n",fb->fb_base);
	if(fb->fb_base == MAP_FAILED)
	{
		close(fb->fbh);
		if (fb->fb_tmp != NULL) {
			free(fb->fb_tmp);
			fb->fb_tmp = NULL;
		}
		perror("mmap");
		return -1;
	}
#endif
	return 0;
}

void fb_release(struct framebuffer *fb)
{
	printf("release fb %x \n", fb);

	if (fb != NULL) {
		#if VIDEODISP
		SunxiMemPfree(fb->pMemops, fb->pVirBuf);
		SunxiMemClose(fb->pMemops);
		#elif VID_DRM
		drm_deinit();
		//fb->fb_tmp = NULL;
	#else
		set_displayed_framebuffer(fb,0);
		close(fb->fbh);
		
	#endif
		if (fb->fb_tmp != NULL) {
			free(fb->fb_tmp);
			fb->fb_tmp = NULL;
		}
	}
}
void fb_flush(struct framebuffer *fb)
{
#if VIDEODISP
int i=0;
unsigned char* p = fb->pVirBuf;
unsigned char* src = fb->fb_tmp;
for(i=0;i<480;i++)
{	memcpy(p,src, 800*4);
p+=800*4;
src+=1024*4;
}
//memcpy(fb->pVirBuf, fb->fb_tmp, 800 * 480 * fb->cpp);
/**SunxiMemFlushCache(fb->pMemops, (void*)fb->pVirBuf, fb->screen_x * fb->screen_y * fb->cpp);

*/
#elif VID_DRM
 memcpy(fb->fb_base,fb->fb_tmp, fb->screen_x * fb->screen_y * 4);
#else
		if (double_buffered != 0)
		{
			memcpy(fb->fb_base + ((1-fb->displayed_buffer)*fb->screen_y *  fb->x_stride ) * fb->cpp, fb->fb_tmp, fb->screen_x * fb->screen_y * fb->cpp);
			set_displayed_framebuffer(fb, 1-fb->displayed_buffer);
		}
		else {
			memcpy(fb->fb_base, fb->fb_tmp, fb->fix.smem_len);
		}
#endif
}

int openFB(const char *name)
{
	int fh;
	char *dev;

	if(name == NULL)
	{
		dev = getenv("FRAMEBUFFER");
		if(dev) name = dev;
		else name = DEFAULT_FRAMEBUFFER;
	}

	if((fh = open(name, O_RDWR)) == -1)
	{
		fprintf(stderr, "open %s: %s\n", name, strerror(errno));
		return -1;
	}
	return fh;
}

void closeFB(int fh)
{
	close(fh);
}

void getVarScreenInfo(int fh, struct fb_var_screeninfo *var)
{
	if(ioctl(fh, FBIOGET_VSCREENINFO, var))
	{
		fprintf(stderr, "ioctl FBIOGET_VSCREENINFO: %s\n", strerror(errno));
		exit(1);
	}
}

void setVarScreenInfo(int fh, struct fb_var_screeninfo *var)
{
	if(ioctl(fh, FBIOPUT_VSCREENINFO, var))
	{
		fprintf(stderr, "ioctl FBIOPUT_VSCREENINFO: %s\n", strerror(errno));
		exit(1);
	}
}

void getFixScreenInfo(int fh, struct fb_fix_screeninfo *fix)
{
	if (ioctl(fh, FBIOGET_FSCREENINFO, fix))
	{
		fprintf(stderr, "ioctl FBIOGET_FSCREENINFO: %s\n", strerror(errno));
		exit(1);
	}
}

