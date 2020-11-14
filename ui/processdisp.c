#include <stdio.h>


#include "fbviewer.h"


void* convertRGB2FB(int fh, unsigned char *rgbbuff, unsigned long count, int bpp, int *cpp);


struct framebuffer g_fb={0};

int initfb()
{
	if(g_fb.fb_tmp)return 0;
	if (fb_init(&g_fb) < 0) {
		printf( "fb_init failed.\n");
		return -1;
	}
	//memset(g_fb.fb_base, 0x00, g_fb.fix.smem_len);
	
return 0;
}
char is_hdmi()
{
	return g_fb.ishdmi;
}
struct framebuffer *getframe()
{
  return &g_fb;
}
void *get_framebuffer()
{
	return g_fb.fb_base;
}
int get_frame_width()
{
return g_fb.screen_x;
}
int get_frame_height()
{
return g_fb.screen_y;
}

void deinitfb()
{
	fb_release(&g_fb);
	memset(&g_fb,0, sizeof(struct framebuffer));
}
inline static unsigned char make8color(unsigned char r, unsigned char g, unsigned char b)
{
	return (
	(((r >> 5) & 7) << 5) |
	(((g >> 5) & 7) << 2) |
	 ((b >> 6) & 3)	   );
}

inline static unsigned short make15color(unsigned char r, unsigned char g, unsigned char b)
{
	return (
	(((r >> 3) & 31) << 10) |
	(((g >> 3) & 31) << 5)  |
	 ((b >> 3) & 31)		);
}

inline static unsigned short make16color(unsigned char r, unsigned char g, unsigned char b)
{
	return (
	(((r >> 3) & 31) << 11) |
	(((g >> 2) & 63) << 5)  |
	 ((b >> 3) & 31)		);
}

void* convertRGB2FB(int fh, unsigned char *rgbbuff, unsigned long count, int bpp, int *cpp)
{
	unsigned long i;
	void *fbbuff = NULL;
	u_int8_t  *c_fbbuff;
	u_int16_t *s_fbbuff;
	u_int32_t *i_fbbuff;

	switch(bpp)
	{
	case 8:
		*cpp = 1;
		c_fbbuff = (unsigned char *) malloc(count * sizeof(unsigned char));
		for(i = 0; i < count; i++)
		c_fbbuff[i] = make8color(rgbbuff[i*3], rgbbuff[i*3+1], rgbbuff[i*3+2]);
		fbbuff = (void *) c_fbbuff;
		break;
	case 15:
		*cpp = 2;
		s_fbbuff = (unsigned short *) malloc(count * sizeof(unsigned short));
		for(i = 0; i < count ; i++)
		s_fbbuff[i] = make15color(rgbbuff[i*3], rgbbuff[i*3+1], rgbbuff[i*3+2]);
		fbbuff = (void *) s_fbbuff;
		break;
	case 16:
		*cpp = 2;
		s_fbbuff = (unsigned short *) malloc(count * sizeof(unsigned short));
		for(i = 0; i < count ; i++)
		s_fbbuff[i] = make16color(rgbbuff[i*3], rgbbuff[i*3+1], rgbbuff[i*3+2]);
		fbbuff = (void *) s_fbbuff;
		break;
	case 24:
		*cpp = 4;
		i_fbbuff = (unsigned int *) malloc(count * sizeof(unsigned int));
		for(i = 0; i < count ; i++)
		i_fbbuff[i] = ((0xFF << 24) & 0xFF000000) |
				((rgbbuff[i*3] << 16) & 0xFF0000) |
				((rgbbuff[i*3+1] << 8) & 0xFF00) |
				(rgbbuff[i*3+2] & 0xFF);
		fbbuff = (void *) i_fbbuff;
		break;
	case 32:
		*cpp = 4;
		i_fbbuff = (unsigned int *) malloc(count * sizeof(unsigned int));
		for(i = 0; i < count ; i++)
		i_fbbuff[i] = ((rgbbuff[i*3+3] << 24) & 0xFF000000) |
				((rgbbuff[i*3] << 16) & 0xFF0000) |
				((rgbbuff[i*3+1] << 8) & 0xFF00) |
				(rgbbuff[i*3+2] & 0xFF);
		fbbuff = (void *) i_fbbuff;
		break;
	default:
		fprintf(stderr, "Unsupported video mode! You've got: %dbpp\n", bpp);
		exit(1);
	}
	return fbbuff;
}

static int loadimage(char *filename, struct Image *i)
{
	int (*load)(char *, unsigned char *, unsigned char **, int, int);
	unsigned char * image = NULL;
	unsigned char * alpha = NULL;

	int x_size, y_size;

#ifdef FBV_SUPPORT_PNG
	if(fh_png_id(filename))
	if(fh_png_getsize(filename, &x_size, &y_size) == FH_ERROR_OK)
	{
		load = fh_png_load;
		goto identified;
	}
#endif

#ifdef FBV_SUPPORT_JPEG
	if(fh_jpeg_id(filename))
	if(fh_jpeg_getsize(filename, &x_size, &y_size) == FH_ERROR_OK)
	{
		load = fh_jpeg_load;
		goto identified;
	}
#endif

#ifdef FBV_SUPPORT_BMP
	if(fh_bmp_id(filename))
	if(fh_bmp_getsize(filename, &x_size, &y_size) == FH_ERROR_OK)
	{
		load = fh_bmp_load;
		goto identified;
	}
#endif
	fprintf(stderr, "%s: Unable to access file or file format unknown.\n", filename);
	return(1);

identified:
	if(!(image = (unsigned char*)malloc(x_size * y_size * 3)))
	{
		fprintf(stderr, "%s: Out of memory.\n", filename);
		return -1;
	}
	if(load(filename, image, &alpha, x_size, y_size) != FH_ERROR_OK)
	{
		fprintf(stderr, "%s: Image data is corrupt?\n", filename);
		free(image);
		return -1;
	}
	//if(!opt_alpha)
	//{
		//free(alpha);
		//alpha = NULL;
	//}


	i->width = x_size;
	i->height = y_size;
	i->rgb = image;
	i->alpha = alpha;
	i->do_free = 0;

	return 0;
}

int load_image(char *filename, struct Image *i, int dispx,int dispy, char iscov)
{
	int ret = 0;

	if (!i->fbbuffer) {
		if (!i->rgb) {
			ret = loadimage(filename, i);
			if (ret != 0) {
				fprintf(stderr, "load_image [%s] failed\n", filename);
				return ret;
			}
		}
		if(iscov) {
			i->fbbuffer = (unsigned char*)convertRGB2FB(0, i->rgb, i->width * i->height, g_fb.var.bits_per_pixel, &g_fb.cpp);
			free(i->rgb);
			i->rgb = NULL;
		} else {
			i->fbbuffer = i->rgb;
		}
		
		i->x_offs = dispx;
		i->y_offs = dispy;
	}
	return ret;
}

void unload_image(struct Image *i)
{
	if(i->fbbuffer)free(i->fbbuffer);
	if(i->alpha)free(i->alpha);
	//if(i->rgb)free(i->rgb);
	i->fbbuffer =NULL;
	i->alpha =NULL;
	i->rgb = NULL;
	memset(i,0,sizeof(i));
}

void drawfullscreen(struct Image *i)
{
	memcpy(g_fb.fb_tmp, i->fbbuffer, i->width*i->height*4);
}

void drawpngrect(unsigned char* framebuf,unsigned char* p, int h, int w,int scw,int xoffs, unsigned char *alp)
{
	int i,j; 
	unsigned char r,g,b;
	unsigned char r0,g0,b0;
	unsigned char alpha;
	for(j=0;j<h;j++)
	{
		for(i=0;i<w;i++)
		{
			r=p[i*3];
			g =p[i*3+1];
			b =p[i*3+2];
			alpha=alp[i];
			b0= framebuf[xoffs+i*4];
			g0= framebuf[xoffs+i*4+1];
			r0= framebuf[xoffs+i*4+2];
			
			framebuf[xoffs+i*4]=(((unsigned int)(alpha) * b) + (unsigned int)(255 - alpha) * b0) / 255;//b
			framebuf[xoffs+4*i+1]=(((unsigned int)(alpha) * g) + (unsigned int)(255 - alpha) * g0) / 255;//g
			framebuf[xoffs+4*i+2]=(((unsigned int)(alpha) * r) + (unsigned int)(255 - alpha) * r0) / 255;//r	
		}
		
		p += w*3;
		alp +=w;
		framebuf += scw*4;
	}
}

void drawjpgrect(unsigned char* framebuf,unsigned char* p, int h, int w,int scw,int xoffs)
{
	int i,j; 
	for(j=0;j<h;j++)
	{
		memcpy(framebuf+xoffs,p,w*4);
		p += w*4;
		framebuf += scw*4;
	}

}
void drawrect(struct Image *i)
{
	struct framebuffer *fb = &g_fb;
	unsigned char* framebuf = fb->fb_tmp + i->y_offs*fb->screen_x*4;
	
	//printf("i->x_offs = %d ,i->y_offs = %d\n",i->x_offs,i->y_offs);
	if(i->alpha)
		drawpngrect(framebuf,i->fbbuffer,i->height,i->width,fb->screen_x, 4*(i->x_offs),i->alpha);
	else	
		drawjpgrect(framebuf,i->fbbuffer,i->height,i->width,fb->screen_x, 4*(i->x_offs));
}

void drawrect_gamelist(struct Image *i,int x,int y)
{
	struct framebuffer *fb = &g_fb;
	unsigned char* framebuf = fb->fb_tmp + y*fb->screen_x*4;
	
	//printf("i->x_offs = %d ,i->y_offs = %d\n",i->x_offs,i->y_offs);
	if(i->alpha)
		drawpngrect(framebuf,i->fbbuffer,i->height,i->width,fb->screen_x, 4*(x),i->alpha);
	else	
		drawjpgrect(framebuf,i->fbbuffer,i->height,i->width,fb->screen_x, 4*(x));
}

void drawrect_color(int x,int y,int w,int h,unsigned int argb,unsigned char alpha )
{
	struct framebuffer *fb = &g_fb;
	unsigned char* framebuf = fb->fb_tmp + y*fb->screen_x*4;
	
	int i,j; 
	unsigned int r,g,b;
	unsigned char r0,g0,b0;
	int fbal =255 - alpha;
	b = alpha*(argb&0xff);
	g = alpha*((argb>>8)&0xff);
	r = alpha*((argb>>16)&0xff);
	
	for(j=0;j<h;j++)
	{
		for(i=0;i<w;i++)
		{
			b0= framebuf[x+i*4];
			g0= framebuf[x+i*4+1];
			r0= framebuf[x+i*4+2];
			
			framebuf[x+i*4]=( b + fbal * b0) / 255;//b
			framebuf[x+4*i+1]=( g + fbal * g0) / 255;//g
			framebuf[x+4*i+2]=( r + fbal* r0) / 255;//r	
		}
		framebuf += fb->screen_x*4;
	}
}

void drawcolor(unsigned int argb)
{
	int i;
	unsigned int *p = g_fb.fb_tmp;
	for(i=0;i<g_fb.screen_x*g_fb.screen_y;i++)
		*p++ =  argb;
}

void displayframe()
{
	fb_flush(&g_fb);
}

void preshowpage(const char* file)
{
	
	struct Image imag={0};
	if(load_image(file,  &imag, 0,0,1)!= 0)
		goto exit;
		drawfullscreen(&imag);
		fb_flush(&g_fb);
	exit:
	//printf("%s =%d \n",__FILE__,__LINE__);
	free(imag.fbbuffer);
	imag.fbbuffer = NULL;
	if(imag.alpha)
		free(imag.alpha);
	//printf("%s =%d \n",__FILE__,__LINE__);
}

