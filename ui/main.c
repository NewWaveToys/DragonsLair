/*
 * This is a every simple sample for MiniGUI.
 * It will create a main window and display a string of "Hello, world!" in it.
 */
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/poll.h>
//#include <libudev.h>
#include <linux/types.h>
#include <linux/input.h>
#include <sys/time.h> 
#include <sys/types.h>
#include <unistd.h>
#include <math.h>

#include <sys/mman.h>
#include <linux/fb.h>
#include <fcntl.h>
#include <stdbool.h>



int fd;
void *bits = NULL;
static struct fb_var_screeninfo vi;
  static struct fb_fix_screeninfo fi;
void init_fb()
{
	int screensize;
	int mmapsize;
	const int PADDING = 4096;
    fd = open("/dev/fb0", O_RDWR);
    if (fd < 0) {
        printf("cannot open fb0");
    }
	
 
    
 /*   if (ioctl(fd, FBIOPUT_VSCREENINFO, &vi) < 0) {
        printf("failed to put fb0 info");
       // close(fd);
        //return -1;
    }
 */
    if (ioctl(fd, FBIOGET_FSCREENINFO, &fi) < 0) {
        printf("failed to get fb0 info");
       // close(fd);
       // return -1;
    }
	if (ioctl(fd, FBIOGET_VSCREENINFO, &vi) < 0) {
        printf("failed to get fb0 info");
      //  close(fd);
        
    }
 printf("%d=width %d =%d \n",vi.bits_per_pixel,vi.xres, vi.yres);
 screensize = vi.xres * vi.yres * vi.bits_per_pixel / 8;
    
     mmapsize = (screensize + PADDING - 1) & ~(PADDING-1);
	printf("%s=len %d==%d\n",fi.id,fi.smem_len, fi.line_length);
    bits = mmap(0, mmapsize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (bits == MAP_FAILED) {
        printf("failed to mmap framebuffer\n");
        close(fd);
       // return -1;
    }

}
static int64_t systemTime()
{
    struct timespec t;
    t.tv_sec = t.tv_nsec = 0;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return (t.tv_sec*1000000000LL + t.tv_nsec);
}

int main(int args, const char* argv[])
{
	unsigned char imageData[800*600*4];
	int i=0;
	bool isrun=true;
	int64_t pt;
	printf("============run main==sleep=========\n");
//	sleep(1);
	//gl_init();
	init_fb();
	//inputtest();
	
	setenv("ALSA_CONFIG_DIR","/usr/share/alsa",1);
	printf("========0=====start==\n");
	while(1)
	{
		i++;
		i %= 255;
		pt =  systemTime();
		//video_driver_frame(imageData,480,272, 480<<1);
		//printf("=============draw= %lld =\n",(systemTime()-pt)/1000000);
		//if(i==1)sleep(10);
		usleep(300000);
		memset(imageData,i,800*600*4);
		memcpy(bits,imageData, 800*600*4);
		printf("=============draw= %lld =\n",(systemTime()-pt)/1000000);
		if(i > 253)
		{
			//video_driver_free();
			munmap(bits, fi.smem_len);
			close(fd);
			break;
		}
	}
	printf("============run main===========\n");
	return 0;
}