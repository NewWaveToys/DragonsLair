/*
 * Copyright (c) 2018 rockchip
 *
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "parameter.h"

#define PARAMETER_FILE    "/data/parameter"

#define UI_IMAGE_PATH         ""
static unsigned char ischange = 0;

struct parameter_data
{
	unsigned char version;
	unsigned char language;
	unsigned char screen_brightness;
	unsigned char game_audio;
	unsigned char attract_mode_audio;
	unsigned char diagnostics;
	unsigned char dirks_per_credit;
	unsigned char marquee;
	unsigned char difficulty_level;
	unsigned char joystick_sound;
	unsigned char screen_blanking;
	unsigned char m_switchA;
	unsigned char m_switchB;
};

struct parameter_data para_data;

unsigned char read_screen_brightness()
{
	unsigned int blval=30;

	FILE * fpFile = fopen("/sys/class/backlight/backlight/brightness", "r");
		if (fpFile) {
		
		fscanf(fpFile, "%d",&blval);
		fclose(fpFile);
		}
	if(blval < 20)
		blval = 20;
	return blval&0xff;
}
void write_screen_brightness(unsigned char val)
{
	if(val<10)val=20;
	
	FILE* fpFile = fopen("/sys/class/backlight/backlight/brightness", "w");
	if (fpFile) 
	{	char buf[10]={0};
		
		sprintf(buf, "%d",val);
		fwrite(buf,1, strlen(buf),fpFile);
		fclose(fpFile);
	}

}

unsigned char read_marquee()
{
	unsigned int blval=0;

	FILE *fpFile = fopen("/sys/class/gpio_control/marquee_ctrl", "r");
	if (fpFile) 
	{
		fscanf(fpFile, "%d",&blval);
		fclose(fpFile);
	}
	if(blval < 0)
		blval = 0;
	return blval&0xff;
}
void write_marquee(unsigned char val)
{
	FILE* fpFile = fopen("/sys/class/gpio_control/marquee_ctrl", "w");
	if (fpFile) {		
		fprintf(fpFile, "%d",val);
		fclose(fpFile);
	}
}
void set_audio_state()
{
	if(para_data.game_audio == 1) 
	{
       system("amixer cset name='Playback Path' 'OFF'");
	} 
	else
	{
       system("amixer cset name='Playback Path' 'OFF'");
       system("amixer cset name='Playback Path' 'SPK'");
     }

}

static int parameter_save(void)
{
    FILE * fpFile = 0;

    fpFile = fopen(PARAMETER_FILE, "wb");
    if (!fpFile) {
        printf("create parameter file fail\n");
        return -1;
    }
    fwrite(&para_data, 1, sizeof(struct parameter_data), fpFile);
    fflush(fpFile);
    fsync(fpFile);
    fclose(fpFile);
    system("sync &");

    return 0;
}

int parameter_init(void)
{
    FILE * fpFile = 0;

    memset(&para_data, 0, sizeof(struct parameter_data));

    fpFile = fopen(PARAMETER_FILE, "rb");
    if (!fpFile) {
        printf("open parameter file fail\n");
		parameter_recovery();
        return 0;
    }
    fread(&para_data, 1, sizeof(struct parameter_data), fpFile);
    fclose(fpFile);
	
	//write_marquee(para_data.marquee);
	//write_screen_brightness(para_data.screen_brightness);
	//set_audio_state();
//	printf("=====%d=====%d=%d=\n",	para_data.screen_brightness, 
//		para_data.marquee,para_data.difficulty_level);

    return 0;
}
int credit_change=0;
int level_change = 0;
void reset_change()
{
	ischange = 0;
	credit_change=para_data.dirks_per_credit;
 level_change = para_data.difficulty_level;
}

char game_is_change()
{
	return (ischange!=0)|(credit_change!=para_data.dirks_per_credit) \
			|(level_change != para_data.difficulty_level);
}
void set_change(int bit)
{
	unsigned char tmp = ~ischange;
	//printf("change %x tmp %x \n",ischange,tmp);
	tmp =tmp^(1<<bit);
	ischange =~tmp;
	//printf("end %x = %x\n",ischange,tmp);
}
void parameter_deinit(void)
{
	return 0;
}

int parameter_recovery(void)
{
   	para_data.screen_brightness = read_screen_brightness();
	para_data.game_audio = 0;
	para_data.attract_mode_audio = 1;
	para_data.diagnostics = 0;
	para_data.marquee = read_marquee();
	para_data.dirks_per_credit = 1;
	para_data.difficulty_level = 1;
	para_data.joystick_sound = 1;
	para_data.screen_blanking = 1;
	para_data.m_switchA = 0x22;
	para_data.m_switchB = 0xD8;
	if(para_data.screen_brightness < 20)
	{	//para_data.screen_brightness=20;
	//write_screen_brightness(para_data.screen_brightness);
	}
//printf("=====%d=====%d==\n",	para_data.screen_brightness, para_data.marquee);
    return 0;
}
#if 0
void set_no_keys_val(int val)
{
    para_data.no_keys_val = val;
    parameter_save();
}

int get_no_keys_val(void)
{
    return para_data.no_keys_val;
}
#endif

void set_screen_brightness(unsigned char val)
{
	para_data.screen_brightness = val;
    parameter_save();
	write_screen_brightness(val);
}
unsigned char get_screen_brightness(void){return para_data.screen_brightness;}

void set_game_audio(unsigned char val)
{
	para_data.game_audio = val;
    parameter_save();
	set_audio_state();
}
unsigned char get_game_audio(void){return para_data.game_audio;}

void set_attract_audio(unsigned char val)
{
	para_data.attract_mode_audio = val;
	//parameter_save();
	//set_change(1);
}

unsigned char get_attract_audio(void){return para_data.attract_mode_audio;}

void set_marquee(unsigned char val)
{//printf("%s %d \n",__FUNCTION__ ,val);
	para_data.marquee = val;
	parameter_save();
	write_marquee(val);
}

unsigned char get_marquee(void){return para_data.marquee;}

void set_dirks_per_credit(unsigned char val)
{
	para_data.dirks_per_credit = val;
    parameter_save();
	//set_change(2);
}
unsigned char get_dirks_per_credit(void){return para_data.dirks_per_credit;}

void set_difficulty_level(unsigned char val)
	{
		para_data.difficulty_level = val;
		parameter_save();
		//set_change(3);
	}

unsigned char get_difficulty_level(void){return para_data.difficulty_level;}

void set_joystick_sound(unsigned char val)
	{
	para_data.joystick_sound = val;
    parameter_save();
}
unsigned char get_joystick_sound(void){return para_data.joystick_sound;}

void set_screen_blanking(unsigned char val)
	{
	para_data.screen_blanking = val;
    parameter_save();
	//set_change(4);
}
unsigned char get_screen_blanking(void){return para_data.screen_blanking;}


char *get_ui_image_path(void)
{
    return UI_IMAGE_PATH;
}

