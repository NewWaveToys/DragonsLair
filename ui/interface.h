#ifndef __INTERFACE_H
#define __INTERFACE_H
#include <time.h>

#define KEY_P1_FUNC 	310
#define KEY_P2_FUNC		311
#define KEY_FIRE_FUNC 	304
#define KEY_B_FUNC		305
#define KEY_UP_FUNC		103
#define KEY_DOWN_FUNC 	108
#define KEY_LEFT_FUNC	105
#define KEY_RIGHT_FUNC	106

#define KEY_X_FUNC 	307
#define KEY_Y_FUNC 	308
#define KEY_C_FUNC	315
#define KEY_Z_FUNC	314
#define KEY_L_FUNC	312
#define KEY_R_FUNC	313

#include "parameter.h"

#define KEY_B_V 		1<<0
#define KEY_UP_V 		1<<1
#define KEY_DOWN_V 		1<<2
#define KEY_LEFT_V 		1<<3
#define KEY_RIGHT_V 	1<<4
#define KEY_Y_V			1<<5
#define KEY_P1_V 		1<<6
#define KEY_P2_V		1<<7

#define KEY_FIRE_V 		1<<8
#define KEY_X_V			1<<9
#define KEY_C_V			1<<10
#define KEY_Z_V			1<<11
#define KEY_L_V			1<<12
#define KEY_R_V			1<<13

#define VERSION_FILE   "/etc/version"
#define TESTMODE_FILE   "/sdcard/game/GBX75_test.txt"

#define UI_IMAGE_PATH	"/usr/local/share/minigui/res/images/"
extern struct page testmode_page;
extern struct page testqa_page;
extern struct page browser_page;
extern int disablekey;

#define FILE_NUM_PERPAGE  20

struct page
{
const char* name;
int (*init)(const char * dir);
int (*deinit)();
 int (*onkey)(unsigned short code , unsigned int value);
 int (*onrender)();
 void (*reset)();
 int (*pause)(int flag);
 int (*resume)(int flag);
 
};

struct pitem
{
	char* normal;
	char* press;
	int x;
	int y;
	unsigned short key;
};

typedef struct _RECT
{
    int x;
    int y;
    int w;
    int h;
} RECT;

struct games
{
	char* emu;
	char* rom;
};

typedef int (*sound_end)();

static unsigned long GetTickCount()
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (ts.tv_sec * 1000 + ts.tv_nsec / 1000000);
}

#if 1
#define BROWSER_PATH_GAME     "/sdcard/game"
#define DEFRETROARCH_PATH   "/usr/lib/libretro/retroarch"
#define DEFRETROARCH   "/usr/lib/libretro/retroarch/retroarch.cfg"
#define NES_CFG   "nes.cfg"
#define SFC_CFG   "sfc.cfg"
#define GBA_CFG   "gba.cfg"
#define MD_CFG   "md.cfg"
#define A2600_CFG   "a2600.cfg"
#define A7800_CFG   "a7800.cfg"

struct file_node
{
    struct file_node *pre_node;
    struct file_node *next_node;
    char *name;
    int type;
};

struct directory_node
{
    struct directory_node *pre_node;
    struct directory_node *next_node;
    struct file_node *file_node_list;
    char *path;
    int total;
    int file_sel;
};

enum filter_filetype {
    FILTER_FILE_NO = 0,
    FILTER_FILE_MUSIC = 1,
    FILTER_FILE_GAME = 2,
    FILTER_FILE_PIC = 3,
    FILTER_FILE_ZIP = 4,
    FILTER_FILE_VIDEO = 5
};

enum filetype {
    FILE_FOLDER = 0,
    FILE_MUSIC = 1,
    FILE_GAME = 2,
    FILE_PIC = 3,
    FILE_ZIP = 4,
    FILE_VIDEO = 5,
    FILE_OTHER = 6,
    FILE_TYPE_MAX
};
	
typedef enum {
	SIMULATOR_UNKNOW = 0,
	SIMULATOR_1 = 1,
	SIMULATOR_2 = 2,
	SIMULATOR_3 = 3,
	SIMULATOR_4 = 4,
	SIMULATOR_5 = 5,
	SIMULATOR_6 = 6,
	SIMULATOR_7 = 7,
	SIMULATOR_8 = 8,
	SIMULATOR_9 = 9,
	SIMULATOR_10 = 10,
}eSimulater;

#endif


#endif
