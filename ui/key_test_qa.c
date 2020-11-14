#include <stdio.h>
#include "interface.h"
#include "fbviewer.h"
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include "rthreads/rthreads.h"

static unsigned char testqa_change = 0;
static unsigned char qa_key_test_status = 1;
#define NUMQA 8
#define QA_KEY_STATUS 2

static unsigned short gkey_qa = 0;
static unsigned char key_test_success = 0;
static unsigned char qa_key_num_cnt = 0;

struct Image test_key_bmap[NUMQA][QA_KEY_STATUS];

static unsigned short qa_keys[NUMQA]={KEY_B_V,KEY_UP_V,KEY_DOWN_V,KEY_LEFT_V,KEY_RIGHT_V,KEY_Y_V,
	KEY_P1_V,KEY_P2_V};

void qa_preinit(void)
{
	int i;
	char path[256]={0};

	for (i = 0; i < QA_KEY_STATUS; i++) {
		sprintf(path, "%sup_%d.png", UI_IMAGE_PATH,i);
		load_image(path,&test_key_bmap[1][i],593,160,0);
	
		sprintf(path, "%sdown_%d.png", UI_IMAGE_PATH,i);
		load_image(path,&test_key_bmap[2][i],593,347,0);
		
		sprintf(path, "%sleft_%d.png", UI_IMAGE_PATH,i);
		load_image(path,&test_key_bmap[3][i],500,253,0);
		
		sprintf(path, "%sright_%d.png", UI_IMAGE_PATH,i);
		load_image(path,&test_key_bmap[4][i],690,253,0);
		
		sprintf(path, "%sbutton_%d.png", UI_IMAGE_PATH,i);
		load_image(path,&test_key_bmap[6][i],500,520,0);
		load_image(path,&test_key_bmap[7][i],690,520,0);
		load_image(path,&test_key_bmap[0][i],600,70,0);
		load_image(path,&test_key_bmap[5][i],500,430,0);
	}
}

void qa_predestroy(void)
{
	int i,j;
	for (i = 0; i < NUMQA; i ++)
		for (j = 0; j < QA_KEY_STATUS; j++)
			unload_image(&test_key_bmap[i][j]);
}

static int init(const char* dir)
{
	printf("=====init qa test======\n");
	qa_preinit();
	gkey_qa = 0;
	qa_key_num_cnt = 0;
	qa_key_test_status = 1;
	testqa_change = 1;
	key_test_success = 0;
	drawcolor(0xff000000);
	//system("echo 2 > /sys/devices/platform/usb_control/key_mode");
	
	return 0;
}

static int deinit()
{
	//system("echo 0 > /sys/devices/platform/usb_control/key_mode");
	qa_predestroy();
	return 0;
}

static int onkey(unsigned short code, unsigned int value)
{
	if(value==1)
	{
		//printf("code down = %d\n",code);
		switch(code)
		{
			case KEY_UP_FUNC: 	 		gkey_qa  |=KEY_UP_V;break;
			case KEY_DOWN_FUNC: 	 	gkey_qa  |=KEY_DOWN_V;break;
			case KEY_LEFT_FUNC: 	 	gkey_qa  |=KEY_LEFT_V;break;
			case KEY_RIGHT_FUNC:  		gkey_qa  |=KEY_RIGHT_V;break;
			case KEY_P1_FUNC: 	 		gkey_qa  |=KEY_P1_V;break;
			case KEY_P2_FUNC: 	 		gkey_qa  |=KEY_P2_V;break;
			case KEY_FIRE_FUNC: 	 	gkey_qa  |=KEY_FIRE_V;break;
			case KEY_B_FUNC:			gkey_qa  |=KEY_B_V;break;
			case KEY_X_FUNC:			gkey_qa  |=KEY_X_V;break;
			case KEY_Y_FUNC:			gkey_qa  |=KEY_Y_V;break;
			case KEY_C_FUNC:			gkey_qa  |=KEY_C_V;break;
			case KEY_Z_FUNC:			gkey_qa  |=KEY_Z_V;break;
			case KEY_L_FUNC:			gkey_qa  |=KEY_L_V;break;
			case KEY_R_FUNC:			gkey_qa  |=KEY_R_V;break;
		}
		if(qa_key_num_cnt < NUMQA)
		{
			if(qa_keys[qa_key_num_cnt] == gkey_qa)
			{
				qa_key_num_cnt++;
			}
			else if(qa_keys[qa_key_num_cnt] & gkey_qa)
			{
			
			}
			else qa_key_test_status = 0;
		}
		testqa_change = 1;
	}
	else if(value==0)
	{
		//printf("code up = %d\n",code);
		switch(code)
		{
			case KEY_UP_FUNC: 	 		gkey_qa  &=~KEY_UP_V;break;
			case KEY_DOWN_FUNC: 	 	gkey_qa  &=~KEY_DOWN_V;break;
			case KEY_LEFT_FUNC: 	 	gkey_qa  &=~KEY_LEFT_V;break;
			case KEY_RIGHT_FUNC:  		gkey_qa  &=~KEY_RIGHT_V;break;
			case KEY_P1_FUNC: 	 		gkey_qa  &=~KEY_P1_V;break;
			case KEY_P2_FUNC: 	 		gkey_qa  &=~KEY_P2_V;break;
			case KEY_FIRE_FUNC: 	 	gkey_qa  &=~KEY_FIRE_V;break;
			case KEY_B_FUNC:			gkey_qa  &=~KEY_B_V;break;
			case KEY_X_FUNC:			gkey_qa  &=~KEY_X_V;break;
			case KEY_Y_FUNC:			gkey_qa  &=~KEY_Y_V;break;
			case KEY_C_FUNC:			gkey_qa  &=~KEY_C_V;break;
			case KEY_Z_FUNC:			gkey_qa  &=~KEY_Z_V;break;
			case KEY_L_FUNC:			gkey_qa  &=~KEY_L_V;break;
			case KEY_R_FUNC:			gkey_qa  &=~KEY_R_V;break;
		}
		testqa_change = 1;
	}
	
	return 0;
}

static int onrender()
{
	if((key_test_success == 0) && (testqa_change == 1)) {
		int i=0;	
		if(0 == qa_key_test_status ){
			drawcolor(0xff000000);
			gr_color(0xff, 0x00, 0x00, 0xff);
			draw_text("Key fail", -1, -1, 1);
			key_test_success = 1;
		} else if(NUMQA > qa_key_num_cnt && 1 == qa_key_test_status){
			gr_color(0xff,  0xff, 0xff,0xff);
			draw_text("QA/QC mode", -1, 620,1);
			for(i=0;i< NUMQA; i++)
			{
				if(qa_key_num_cnt == i) {
					drawrect(&test_key_bmap[i][1]);
				} else {
					drawrect(&test_key_bmap[i][0]);
				}
			}
		}
		if(NUMQA == qa_key_num_cnt && 1 == qa_key_test_status){
			drawcolor(0xff000000);
			gr_color(0x00, 0xff, 0x00, 0xff);
			draw_text("Key Test Success", -1, -1, 1);
			key_test_success = 1;
		}
		testqa_change = 0;
	}
	return 0;
}

struct page testqa_page =
{
	"testqa",
	init,
	deinit,
	onkey,
	onrender,
};
