#include <stdio.h>
#include "interface.h"
#include "fbviewer.h"
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <errno.h>

#include <time.h>
#include "rthreads/rthreads.h"


static int cur_idx =0;
static char testmdoe_change = 0;
static char mode_isrun = 0;
struct Image g_imag={0};

static void draw_static(int idx, int sx,int sy,int div)
{
	
	gr_color(0x0,  0x0, 0x0,0xff);//rgb a	
	if(is_hdmi())
	{
		drawrect_color(160<<2,sy+idx*div-4,640,div+2,0x00f47920,0xc0);//f47920
	}
	else
	drawrect_color(58<<2,sy+idx*div-2,362,20,0x00f47920,0xc0);//f47920
	draw_text("SCREEN BRIGHTNESS", sx, sy,0);	
	draw_text("GAME AUDIO",		 sx, sy+div,0);	
	draw_text("ATTRACT MODE AUDIO", sx, sy+div*2,0);	
	
	draw_text("DIRKS PER CREDIT", sx, sy+div*3,0);
	draw_text("MARQUEE", 			sx, sy+div*4,0);	
	draw_text("DIFFICULTY LEVEL", sx, sy+div*5,0);	
	draw_text("JOYSTICK SOUND", sx, sy+div*6,0);	
	draw_text("SCREEN BLANKING", sx, sy+div*7,0);	
}
static int getStrSize(const char* s)
{
	//if(get_frame_width()>480)
	//	return 10*strlen(s);

	return 9*strlen(s);
}

static void draw_right(int sx,int sy, int div)
{
const char* str;
char buf[10]={0};
	sprintf(buf, "%d",get_screen_brightness());
//	printf("%s == %d \n", buf, get_screen_brightness());
	draw_text(buf, 	sx-getStrSize(buf), sy,0);	
	
	str =get_game_audio()?"HDMI":"REPLICADE";
	draw_text(str,		 sx-getStrSize(str), sy+div,0);	
	
	str =get_attract_audio()?"ON":"OFF";
	draw_text(str, 	sx-getStrSize(str), sy+div*2,0);	
	
	if(get_dirks_per_credit()==0)str="3";
	else if(get_dirks_per_credit()==1)str="5";
	else str="UNLIMITED";
	draw_text(str, 	sx-getStrSize(str), sy+div*3,0);
	
	str =get_marquee()?"ON":"OFF";
	draw_text(str, 		sx-getStrSize(str), sy+div*4,0);	
	
	if(get_difficulty_level()==0)str="EASY";
	else if(get_difficulty_level()==1)str="MEDIUM";
	else str="HARD";
	draw_text(str, 	sx-getStrSize(str), sy+div*5,0);	
	
	str =get_joystick_sound()?"ON":"OFF";
	draw_text(str, 		sx-getStrSize(str), sy+div*6,0);	
	
	str =get_screen_blanking()?"ON":"OFF";
	draw_text(str, 	sx-getStrSize(str), sy+div*7,0);	
}
void set_option_value(char flag)
{
unsigned char val;
	if(0 == cur_idx)
	{
		val = get_screen_brightness();
		if(flag)
		{
			if(val <230)
			 val +=15;
			else val = 255;
		}
		else val -=15;
		if(val > 255)val =255;
		else if(val < 20)val = 20;
		set_screen_brightness(val);
	}
	else if(1 == cur_idx)
	{
		set_game_audio(!get_game_audio());		
	}
	else if(2 == cur_idx)
	{
		set_attract_audio(!get_attract_audio());		
	}
	else if(3 == cur_idx)
	{
		val = get_dirks_per_credit();
		if(flag)val ++;
		else val --;
		if(val <0)val=2;
		else if(val >2)val=0;
		set_dirks_per_credit(val);		
	}
	else if(4 == cur_idx)
	{
		set_marquee(!get_marquee());		
	}
	else if(5 == cur_idx)
	{
		val =get_difficulty_level();
		if(flag)val ++;
		else val --;
		if(val <0)val=2;
		else if(val >2)val=0;
		set_difficulty_level(val);		
	}
	else if(6 == cur_idx)
	{
		set_joystick_sound(!get_joystick_sound());		
	}
	else if(7 == cur_idx)
	{
		set_screen_blanking(!get_screen_blanking());		
	}
}

static void draw_background()
{
	if(g_imag.fbbuffer)
		drawrect(&g_imag);
	
}
static void draw()
{
	draw_background();
	if(is_hdmi())
	{		
		draw_static(cur_idx, 180, 200,22);
		draw_right(785,200,22);
	}
	else
	{
		draw_static(cur_idx, 75, 105,18);
		draw_right(405,105,18);
	}
}
static int init(const char* dir)
{

	printf("=====init testmode======\n");
	#if 1
	init_text(getframe());
	testmdoe_change = 1;
	cur_idx =0;
	reset_change();
	if(is_hdmi())
	{
		if(load_image("/usr/lib/libretro/games/hdmi_background.jpg",&g_imag,160,0,1)!= 0)
		goto lable_exit;
	}
	else
		
	{
	if(load_image("/usr/lib/libretro/games/lcd_background.jpg",&g_imag,58,0,1)!= 0)
		goto lable_exit;
	
	}
	
lable_exit:
	
	draw();
	#endif
	mode_isrun = 1;
	return 0;
}

static int deinit()
{
	mode_isrun = 0;
#if 1
	deinit_text();
	if(g_imag.fbbuffer)
			free(g_imag.fbbuffer);
		g_imag.fbbuffer = NULL;
		if(g_imag.alpha)
			free(g_imag.alpha);
	drawcolor(0xff000000);
#endif
	return 0;
}

static int onkey(unsigned short code, unsigned int value)
{
printf("%d = %d \n", code ,value);
#if 1
	if(value == 1) {
		
		if(code == 103)
		{
			cur_idx --;//up
			if(cur_idx<0)cur_idx=7;
		}
		else if(code == 108)
		{
			cur_idx ++;//down
			if(cur_idx>7)cur_idx=0;
		}
		else if(code == 105)
		{
			//left
			set_option_value(0);
		}
		else if(code == 106)
		{
			//right
			set_option_value(1);
		}

		draw();
		testmdoe_change = 1;
	}
	#endif
	return 0;
}

static int onrender()
{
	if(mode_isrun)
	{
	#if 1
		displayframe();
	#endif
	}
	usleep(20000);
	return 0;
}

static void onreset()
{
	
}
static int onpause(int flag)
{
	deinit();
}
static int onresume(int flag)
{
	init(NULL);
}

struct page uimode_page =
{
	"uimode",
	init,
	deinit,
	onkey,
	onrender,
	onreset,
	onpause,
	onresume
};
