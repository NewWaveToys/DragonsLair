/*
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 2.1
 * as published by the Free Software Foundation
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/time.h>
#include <dirent.h>
#include <sys/types.h>
#include "interface.h"
#include "fbviewer.h"
#include "rthreads/rthreads.h"
#include <linux/input.h>
#include <memory.h>
#include <sys/prctl.h>
#include <linux/netlink.h>
#include <sys/socket.h>

//#define UI_IMAGE_PATH	"/usr/local/share/minigui/res/images/"
#define KEYTONE_PATH0	"/usr/local/share/minigui/res/music/menu_change.wav"
#define KEYTONE_PATH1	"/usr/local/share/minigui/res/music/select_ding.wav"

#define MAXLEN 2
#define GAME_NUM 10
#define GAME_LINE 2

static int key_vol_sta=0;
static int testkey_num= 0;
unsigned long rungame_time=0;

static int prepaint = 0, startgame = -1;
 int disablekey = 0;
 int needshowui = 0;

static char isSecondpage =0;
static char curindex=0;
static char show_paint = 0;
static char p2_status = 1;

static int messagebox_sel = 0;
static int msgbox_sel = 0;
static int key_fire_sel = 0;
static int sdCardExist = 0;
static char sdCardInit = 0;
static char pagemode_init = 0;

struct Image game_ground_bmap;
struct Image game_icon_bmap[GAME_NUM];
struct Image game_prev_bmap[GAME_NUM];
struct Image list_sel_bmap;
struct Image menu_lcd_background_bmap;

static struct page *pagemode=NULL;
extern struct page uimode_page;
extern struct page gamemode_page;


const RECT game_icon_rect[] = {
	{109,254,218,190},//icon1
	{309,250,218.190},//icon2
	{522,252,218.190},//icon3
	{737,250,218,190},//icon4
	{942,252,218,190},//icon5
	{107,435,218,190},//icon6
	{310,434,218,190},//icon7
	{516,433,226,234},//icon8
	{732,435,217,234},//icon9
	{941,435,217,234},//icon10
};

static void delay_ms(int time)
{
	usleep(1000*time);
}


void showpage(const char* dir,int i)
{
	char path[256]={0};
	
	if(disablekey)return;
	
	//printf("%s =%d \n",__FILE__,__LINE__);
	sprintf(path,"%sgame%d_prev.jpg",UI_IMAGE_PATH,i);
	
	//preshowpage(path);
}

void preloadinit(const char* dir,int p, struct Image *image)
{
    char path[128]={0};
    sprintf(path,"%sgame_%d.png",dir,p);
   // load_image(path,image,0,0,1);
}

#if 0
#define LOGOPATH "/usr/local/share/minigui/res/images/logo.mp4"
static void logo_play(void)
{
    int len;
    char *file_path;
    char cmd[1024];
    if ((access(LOGOPATH,F_OK)) != -1) {
                //LOGOPATH exists
                len = strlen(LOGOPATH) + 4;
                file_path = malloc(len);
                snprintf(file_path, len, "%s",LOGOPATH);
                snprintf(cmd, sizeof(cmd), "/usr/bin/ffplay -x 1280 -y 720 -hide_banner -nostats -fs -vcodec h264 -autoexit \"%s\" ", file_path);
                free(file_path);
                system(cmd);
        }
        else {
                //LOGOPATH don't exists
                printf("----logo.mp4 file don't exists----\n");
        }
}
#endif
#if 0
void preinit(void)
{
	int i;
	char path[256]={0};

	sprintf(path,"%smessagebox_sel.png",UI_IMAGE_PATH);
	load_image(path,&list_sel_bmap,0,60,0);
	
	sprintf(path,"%sbackground.jpg",UI_IMAGE_PATH);
	load_image(path,&game_ground_bmap,0,0,1);

	for(i=0;i<GAME_NUM;i++) {
		sprintf(path,"%sgame_%d.png",UI_IMAGE_PATH,i);
		load_image(path,&game_icon_bmap[i],game_icon_rect[i].x,game_icon_rect[i].y,0);
	}
	
#if 0
	sprintf(path,"%slogo.jpg",UI_IMAGE_PATH);
	preshowpage(path);
	delay_ms(3000);
#else
	logo_play();
	setdrmdisp(getdrmdisp());
#endif

	sprintf(path,"%slegal.jpg",UI_IMAGE_PATH);
	preshowpage(path);	
	delay_ms(3000);
	messagebox_sel = 0;
	curindex = 0;
	//showpage(NULL,curindex);
	drawfullscreen(&game_ground_bmap);
	drawrect(&game_icon_bmap[curindex]);
	system("sync &");
	system("echo 3 > /proc/sys/vm/drop_caches");
}
#endif
void predestroy(void)
{
	int i;
	
	//unload_image(&game_ground_bmap);
	//unload_image(&list_sel_bmap);
	//for(i=0;i<GAME_NUM;i++)
	{
		//unload_image(&game_icon_bmap);
	}
}

static void deytestui(){
	if(pagemode)
		pagemode->deinit();
	if(isSecondpage == 0) {
		drawfullscreen(&game_ground_bmap);
		drawrect(&game_icon_bmap[curindex]);
	} else {
		showpage(NULL,curindex);
	}
	rungame_time = GetTickCount();
	pagemode=NULL;
	//printf("===========deytestui==============\n");
}
#if 0
static void rungame(void)
{
	char path[256]={0};

	disablekey = 1;
	isSecondpage = 0;
	rungame_time = 0;
	keytone_exit();
	gr_color(0xff, 0xff, 0xff, 0xff);
	drawcolor(0xff000000);
	if((startgame == 6) || (startgame == 9)) {
		sprintf(path,"/data/start1.sh %d",startgame);
	} else {
		system("echo 3 > /sys/devices/platform/usb_control/key_mode");	
		sprintf(path,"/data/start.sh %d",startgame);
	}
	system(path);
	system("sync &");
	//printf(" %s \n",path);
	deinitfb();
	initfb();
	drawfullscreen(&game_ground_bmap);
	drawrect(&game_icon_bmap[curindex]);
//	system("echo 0 > /sys/devices/platform/usb_control/key_mode");
//	system("echo 3 > /proc/sys/vm/drop_caches");
	disablekey = 0;
	//printf("========rungame exit========\n");
}
#endif
void setGameIndex(int idx)
{
    startgame = idx;
    prepaint =idx;
}


#if 0
static void parse_sdcard_key(uint16_t code, int value, uint16_t type )
{
	if(value == 1)
	{
		if((KEY_P2_FUNC == code) || (KEY_Y_FUNC == code))
		{
			if(1 == msgbox_sel){
				if(pagemode)
					pagemode->deinit();
				pagemode = &browser_page;
				pagemode->init(NULL);
				pagemode_init = 1;
			}else{
				sdCardExist = 0;
				drawfullscreen(&game_ground_bmap);
				drawrect(&game_icon_bmap[curindex]);
				rungame_time = GetTickCount();
			}
		}
		else
		{
			switch (code) {
				case KEY_LEFT_FUNC:
				case KEY_RIGHT_FUNC:
					msgbox_sel = msgbox_sel ? 0 : 1;
					messagebox_sel = 0;				
					loadkeytone(0);
					keytone_play();					
					break;
			}
		}
	}
}

#endif

void showui()
{
	if(pagemode)pagemode->deinit();
	initfb();
	pagemode =&uimode_page;
	pagemode->init(NULL);
	pagemode_init = 1;
}
void hideui()
{	
	if(!pagemode)return ;
	pagemode_init = 0;
	//gamemode_page.resume(0);
	pagemode->deinit();
	pagemode = NULL;
	deinitfb();
	
}
static void frontend_unix_sighandler(int sig)
{
printf("%s %d \n",__FUNCTION__,sig);
//extern volatile int g_reinit_audio;
//extern volatile int g_reinit_video;
   if (sig == SIGUSR2) {
    //  g_reinit_video = 1;
    if(!pagemode)return;
		pagemode->pause(0);
	//else {
	//	gamemode_page.pause(0);
	//	}
	//sleep(1);
    deinitfb();
	initfb();
	//gamemode_page.reset();
	// if(pagemode)
	 	pagemode->resume(0);
	// else gamemode_page.resume(0);
      return;
   } else if (sig == SIGUSR1) {
    //  g_reinit_audio = 1;
      return;
   }
   else if(sig == SIGTERM)
   	{
   	system("killall -9 daphne.bin");
   	exit(1);
   	}
}
void set_HDIM_sigint()
{
	struct sigaction sa;
	
	   sa.sa_sigaction = NULL;
	   sa.sa_handler   = frontend_unix_sighandler;
	   sa.sa_flags	   = SA_RESTART;
	   sigemptyset(&sa.sa_mask);
	   sigaction(SIGINT, &sa, NULL);
	   sigaction(SIGTERM, &sa, NULL);
	
	   // Use SIGUSR2 to reinit video, SIGUSR1 to reinit audio.
	   sigaction(SIGUSR1, &sa, NULL);
	   sigaction(SIGUSR2, &sa, NULL);

}
int onkey(unsigned short code, unsigned int value)
{
if(disablekey)return 0;

	if(code == 107&&value==1)
	{	
		//quitflag = 1;
	//	deinit_keyevent();
		//is_run = 0;
		disablekey = 1;
		hideui();
		printf("%d == %d \n", code, value);
		return 0;
	}
	//printf("%d %d \n", code, value);
if(pagemode)pagemode->onkey(code, value);
	return 0;
}

int main(int argc, char **argv)
{
	
	int i,ret;
	
	unsigned long prev_time=0;
	
	//fprintf(stderr,"======init=====\n");
	//if(0 != initfb())return 0;
	//	drawcolor(0xff000000);//draw black screen
	set_HDIM_sigint();
	
#if 0
	ret = loadconf("/data/conf.txt");
	if(!ret)
	{
		init_text(getframe());
		drawcolor(0xff000000);
		gr_color(0xff,  0xff, 0xff,0xff);//rgb a			
			
		draw_text("load config error!", -1, -1,0);			
		displayframe();
		
		deinit_text();
		deinitfb();
		return 0;
	}	
	//GAME_NUM = getGameNum();
#endif

	#if 0
	if(testmodeKey()){
		setIndex(5);
		usleep(60000);
		setIndex(1);
		init_text(getframe());
		pagemode = &testcolor_page;
		pagemode->init(NULL);
	}
	else
	#endif
	//{	
	//	preinit();
	//	displayframe();
	//}
	
	//init_text(getframe());
	disablekey = 1;
	 
	//minput_monitor_thread = sthread_create(input_monitor_thread, NULL);
	//meventloop = sthread_create(EventLoop, NULL);
	//if (sdcard_check()==0)
	//	sdCardExist = 0;
	//else
	//	sdCardExist = 1;
	//printf("%s sdCardExist=%d\n",__func__,sdCardExist);

	rungame_time = GetTickCount();
	parameter_init();
	
	//open_alsa();
	//gamemode_page.init(NULL);
	disablekey = 0;
	init_keyevent();
	while(1)
	{
		/*if(disablekey) {
			usleep(5000);
			continue;
		}*/
		#if 1
		
		if(pagemode) 
		{
			if(pagemode_init)
			ret = pagemode->onrender();
		}
		else 
		{
			disablekey = 1;
			//system("./daphne.bin");
			system("/usr/lib/libretro/games/daphne.bin");
			showui();
			disablekey = 0;
			/*if(gamemode_page.onrender()&&needshowui)
			{
				needshowui = 0;
				showui();
				disablekey = 0;
			}*/
		}
		#else
		if(sdCardExist == 0) {
			if(sdCardInit == 1) {
				//printf("%s sdCardExist=%d sdCardInit=%d\n",__func__,sdCardExist,sdCardInit);
				sdCardInit = 0;
	           	if(pagemode)
					deytestui();
				pagemode_init = 0;
				show_paint = 1;
	           	msgbox_sel = 0;
	           	messagebox_sel = 0;
			}
			if(show_paint == 1) {
				curindex = prepaint;
				if(isSecondpage == 0) {
					drawfullscreen(&game_ground_bmap);
					drawrect(&game_icon_bmap[curindex]);
				} else {
					showpage(NULL,curindex);
				}
				//printf( "show index: %d\n", curindex);
				show_paint = 0;
				rungame_time = GetTickCount();
			}
			if(isSecondpage == 0){
				if(rungame_time != 0 && (GetTickCount() - rungame_time) > 200000){
					isSecondpage = 1;
					prepaint = curindex;
					show_paint = 1;
					rungame_time = GetTickCount();
				}
			}else if(rungame_time != 0 && (GetTickCount() - rungame_time) > 200000){					
					setGameIndex(curindex);
					rungame();
			}
		}else {
			if(pagemode) {
				if(pagemode_init == 1)
					ret = pagemode->onrender();
			} else {
				if(sdcard_file_check()==1){
					if(pagemode)
						pagemode->deinit();
					pagemode = &testmode_page;
					pagemode->init(NULL);
					pagemode_init = 1;
					sdCardInit = 1;
				} else {
					if(messagebox_sel == 0){
						messagebox_sel = 1;
						drawcolor(0xff000000);
						gr_color(0xff,0xff, 0xff,0xff);//rgb a
						rungame_time = 0;
						draw_text("Micro SD Card Mounted", 300, 250,1);
						draw_text("Local Storage", 300, 330,1);
						draw_text("Micro SD Card", 590, 330,1);
						if(msgbox_sel == 0)
							drawrect_gamelist(&list_sel_bmap ,270,300,0 );
						else
							drawrect_gamelist(&list_sel_bmap ,560,300,0 );
						sdCardInit = 1;
					}
				}
			}
		}
#endif
		//======flush=========
		//displayframe();
		//usleep(5000);
	}

	deinit_loopkeyevent();
	
	//close_alsa();
	deinitfb();
	deinit_text();
	predestroy();
	
	//freeconf();
	//...
return 0;
}

