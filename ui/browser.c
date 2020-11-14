#include <stdio.h>
#include "fbviewer.h"
#include <string.h>
#include "interface.h"

#include <stdlib.h>
#include <time.h>
#include <sys/time.h> 
#include <math.h>
#include <sys/ioctl.h>
#include <sys/prctl.h>

#include<sys/stat.h>
#include<sys/types.h>
#include<dirent.h>
#include <unistd.h>


struct Image list_bmap;
struct Image browser_bg_bmap;


static int file_list = 0;
static int file_sel = 0;
static struct directory_node *dir_node = 0;
static struct directory_node *cur_dir_node = 0;
static int filter_type = 0;

static char game_file_ext_name[][10] = 
{
	".nes",
	".fds",
	".bin",
	".smd",
	".gg",
	".gen",
	".sfc",
	".smc",
	".gba",
	".gbc",
	".gb",
	".a26",
	".a78"
};

static int check_file_type(char *name)
{
    int i;
	char *filename = strrchr(name,'.');
	if(filename != NULL) {
	    for (i = 0; i < sizeof(game_file_ext_name) / sizeof(game_file_ext_name[0]); i++) {
	        if (strcasestr(filename, game_file_ext_name[i]) != 0)
	            return FILE_GAME;
	    }
	}

    return FILE_OTHER;
}

static void get_file_list(struct directory_node *node)
{
    DIR *dp;
    struct dirent *entry;
    struct stat statbuf;
    struct file_node *file_node;
    int len;
    char *dir = node->path;
    
    file_node = node->file_node_list;
    if((dp=opendir(dir))==NULL){
        printf("open %s error1\n",dir);
        return ;
    }

    chdir(dir);
    while((entry=readdir(dp))!=NULL){
        lstat(entry->d_name,&statbuf);
        if(S_ISDIR(statbuf.st_mode)){
            struct file_node *file_node_temp;
            char *name;

            //printf("name:%s/  len = %d  type:%d  inode:%d\n", entry->d_name, strlen(entry->d_name),
                   //statbuf.st_mode,statbuf.st_size,entry->d_ino);
            if(strcmp(".",entry->d_name)==0 || strcmp("..",entry->d_name)==0)
                continue;
            file_node_temp = malloc(sizeof(struct file_node));
            memset(file_node_temp, 0, sizeof(struct file_node));
            len = strlen(entry->d_name) + 2;
            name = malloc(len);
            memset(name, 0, len);
            sprintf(name, "%s", entry->d_name);
            file_node_temp->pre_node = file_node;
            file_node_temp->type = FILE_FOLDER;
            file_node_temp->name = name;
            if (file_node) {
                file_node->next_node = file_node_temp;
                file_node_temp->pre_node = file_node;
            } else {
                node->file_node_list = file_node_temp;
            }
            file_node = file_node_temp;
            //printf("file_node_temp->pre_node = 0x%x\n", file_node_temp->pre_node);
            //printf("file_node_temp->name = %s\n", file_node_temp->name);
            node->total ++;
        }
    }
    chdir("..");
    closedir(dp);
    if((dp=opendir(dir))==NULL){
        printf("open %s error2\n",dir);
        return ;
    }
    chdir(dir);
    while((entry=readdir(dp))!=NULL){
        lstat(entry->d_name,&statbuf);
        if(!S_ISDIR(statbuf.st_mode)){
            struct file_node *file_node_temp;
            char *name;
            int type;

            //printf("name:%s/  len = %d  type:%d  inode:%d\n",entry->d_name, strlen(entry->d_name),
                   //statbuf.st_mode,statbuf.st_size,entry->d_ino);
            type = check_file_type(entry->d_name);
            if ((filter_type != FILTER_FILE_NO) && (filter_type != type))
                continue;

            file_node_temp = malloc(sizeof(struct file_node));
            memset(file_node_temp, 0, sizeof(struct file_node));
            len = strlen(entry->d_name) + 2;
            name = malloc(len);
            memset(name, 0, len);
            sprintf(name, "%s", entry->d_name);
            file_node_temp->pre_node = file_node;
            file_node_temp->name = name;
            file_node_temp->type = type;//check_file_type(name);
            if (file_node) {
                file_node->next_node = file_node_temp;
                file_node_temp->pre_node = file_node;
            } else {
                node->file_node_list = file_node_temp;
            }
            file_node = file_node_temp;
            node->total ++;
        }
    }
    chdir("..");
    closedir(dp);
    //printf("node->total = %d\n", node->total);
}

static struct directory_node *new_dir_node(char *dir)
{
    int len;
    struct directory_node *dir_node_temp;

    dir_node_temp = malloc(sizeof(struct directory_node));
    memset(dir_node_temp, 0, sizeof(struct directory_node));
    len = strlen(dir) + 2;
    dir_node_temp->path = malloc(len);
    memset(dir_node_temp->path, 0, len);
    sprintf(dir_node_temp->path, "%s", dir);
    //printf("================dir_node_temp->patch = %s\n", dir_node_temp->patch);
    get_file_list(dir_node_temp);

    return dir_node_temp;
}

static struct directory_node *free_dir_node(struct directory_node *node)
{
    struct directory_node *dir_node_temp;
    struct file_node *file_node_temp;
	
    if (node == 0)
        return 0;

    dir_node_temp = node->pre_node;
    
    if (node->path) {
        free(node->path);
        node->path = 0;
    }
    if (node->pre_node){
        node->pre_node->next_node = 0;
	}
    file_node_temp = node->file_node_list;
    while (file_node_temp) {
        struct file_node *file_node_next = file_node_temp->next_node;

        if (file_node_temp->name)
            free(file_node_temp->name);

        file_node_temp = file_node_next;
    }
    free(node);

    return dir_node_temp;
}

static void free_file_list(struct directory_node *node)
{
    struct file_node *file_node_temp;
    
    if (node == 0)
        return;
	
    if (node->next_node)
        free_file_list(node->next_node);

    if (node->path) {
        free(node->path);
        node->path = 0;
    }
    if (node->pre_node)
        node->pre_node->next_node = 0;
    file_node_temp = node->file_node_list;
    while (file_node_temp) {
        struct file_node *file_node_next = file_node_temp->next_node;

        if (file_node_temp->name)
            free(file_node_temp->name);

        file_node_temp = file_node_next;
    }
    free(node);
}

static void file_list_init(void)
{
	//printf("=======file_list_init  =======\n");
	if (filter_type == FILTER_FILE_GAME)
        dir_node = new_dir_node(BROWSER_PATH_GAME);
    else
        dir_node = new_dir_node(BROWSER_PATH_GAME);
	
    cur_dir_node = dir_node;
}

static void file_list_deinit(void)
{
    cur_dir_node = 0;

	if(dir_node) {
    	free_file_list(dir_node);
	}

    dir_node = 0;
}

static void enter_folder(struct directory_node *node)
{
    int i;
    struct file_node *file_node_temp = node->file_node_list;
    for (int i = 0; i < node->file_sel; i++) {
        if (file_node_temp)
            file_node_temp = file_node_temp->next_node;
        else
            return;
    }
    if (file_node_temp->type == FILE_FOLDER) {
        int len;
        char *dir;
        struct directory_node *dir_node_temp;

        len = strlen(node->path);
        len += strlen(file_node_temp->name);
        len += 2;
        dir = malloc(len);
        memset(dir, 0, len);
        sprintf(dir, "%s/%s", node->path, file_node_temp->name);
		//printf("=====%s    file_node_temp->name: %s  ======\n",__func__,file_node_temp->name);
        dir_node_temp = new_dir_node(dir);
        cur_dir_node->next_node = dir_node_temp;
        dir_node_temp->pre_node = cur_dir_node;
        cur_dir_node = dir_node_temp;
        free(dir);
    }else if (file_node_temp->type == FILE_GAME) {
        char cmd[512];
		int sel = 0;
		char path[256]={0};
		char cfgfilepath[256]={0};
		char gamefilecfg[256]={0};
		
		char *gamefile_suffix = strrchr(file_node_temp->name,'.') + 1;

		if(strcasestr(gamefile_suffix,"nes") || strcasestr(gamefile_suffix,"fds")) {//NES
			sel = SIMULATOR_1;
			sprintf(gamefilecfg,"%s",NES_CFG);
		} else if(strcasestr(gamefile_suffix,"bin") || strcasestr(gamefile_suffix,"smd") || strcasestr(gamefile_suffix,"gg") || strcasestr(gamefile_suffix,"gen")) {//MD
			sel = SIMULATOR_2;
			sprintf(gamefilecfg,"%s",MD_CFG);
		} else if(strcasestr(gamefile_suffix,"sfc") || strcasestr(gamefile_suffix,"smc")) {//SFC
			sel = SIMULATOR_3;
			sprintf(gamefilecfg,"%s",SFC_CFG);
		} else if(strcasestr(gamefile_suffix,"gba") || strcasestr(gamefile_suffix,"gbc") || strcasestr(gamefile_suffix,"gb")) {//GBA
			sel = SIMULATOR_4;
			sprintf(gamefilecfg,"%s",GBA_CFG);
		} else if(strcasestr(gamefile_suffix,"a26")) {//A2600
			sel = SIMULATOR_5;
			sprintf(gamefilecfg,"%s",A2600_CFG);
		} else if(strcasestr(gamefile_suffix,"a78")) {//A7800
			sel = SIMULATOR_6;
			sprintf(gamefilecfg,"%s",A7800_CFG);
		} else {
			sel = SIMULATOR_UNKNOW;
			printf(" unknow game file!\n");
		}

		//printf("gamefilecfg=%s gamefile_suffix=%s\n",gamefilecfg,gamefile_suffix);

		sprintf(cfgfilepath,"%s/%s",DEFRETROARCH_PATH,gamefilecfg);
		if ((access(cfgfilepath,F_OK)) != -1) {
			//cfgfilepath exists
		}
		else {
			//cfgfilepath don't exists
			sprintf(cfgfilepath,"%s",DEFRETROARCH);
		}
		disablekey = 1;
		keytone_exit();		
		sprintf(path, "%s/%s", node->path, file_node_temp->name);
		sprintf(cmd, "/data/start_sdcard.sh %d \"%s\" \"%s\"", sel,cfgfilepath,path);
		system(cmd);
		deinitfb();
		initfb();
		system("sync &");
		system("echo 3 > /proc/sys/vm/drop_caches");
		file_list = 0;
		disablekey = 0;
    } 
}

static int init(const char* dir)
{
	char path[256]={0};
	file_list = 1;
	filter_type = FILTER_FILE_GAME;
	//printf("=======init type= %d =======\n",filter_type);
	sprintf(path,"%sbrowser_bg.jpg",UI_IMAGE_PATH);
	load_image(path,&browser_bg_bmap,0,0,1);
	sprintf(path,"%slist_sel.png",UI_IMAGE_PATH);
	load_image(path,&list_bmap,0,60,0);
	drawfullscreen(&browser_bg_bmap);
	file_list_init();
	file_list = 0;
	
	return 0;
}

static int deinit()
{
	file_list_deinit();
	unload_image(&browser_bg_bmap);
	unload_image(&list_bmap);
	chdir("/");
	return 0;
}

static int onkey(unsigned short code, unsigned int value)
{
	if(cur_dir_node != NULL){
		if(value==1)
		{
			switch(code)
			{
				case KEY_B_FUNC:
					if(!strcasecmp(cur_dir_node->path,BROWSER_PATH_GAME))
						break;
	                cur_dir_node = free_dir_node(cur_dir_node);
	                file_list =0;
	                break;
	            case KEY_DOWN_FUNC:
	                if (cur_dir_node->file_sel < (cur_dir_node->total - 1))
	                    cur_dir_node->file_sel++;
	                else
	                    cur_dir_node->file_sel = 0;
					file_list =0;
	                break;
	            case KEY_UP_FUNC:
	                 if (cur_dir_node->file_sel > 0)
	                    cur_dir_node->file_sel--;
	                else
	                    cur_dir_node->file_sel = cur_dir_node->total - 1;
					file_list =0;
	                break;
				case KEY_LEFT_FUNC:
	                if (cur_dir_node->file_sel > 0) {
						if (cur_dir_node->file_sel > (FILE_NUM_PERPAGE - 1))
							cur_dir_node->file_sel = cur_dir_node->file_sel - FILE_NUM_PERPAGE;
						else {
							if (((cur_dir_node->total - 1)/FILE_NUM_PERPAGE*FILE_NUM_PERPAGE + cur_dir_node->file_sel) < (cur_dir_node->total - 1))
								cur_dir_node->file_sel = (cur_dir_node->total - 1)/FILE_NUM_PERPAGE*FILE_NUM_PERPAGE + cur_dir_node->file_sel;
							else
								cur_dir_node->file_sel = (cur_dir_node->total - 1);
						}
					}
					else
						cur_dir_node->file_sel = (cur_dir_node->total - 1)/FILE_NUM_PERPAGE*FILE_NUM_PERPAGE;
					file_list =0;
	                break;
	            case KEY_RIGHT_FUNC:
	                if (cur_dir_node->file_sel < (cur_dir_node->total - 1)) {
						if ((cur_dir_node->file_sel + FILE_NUM_PERPAGE) <= (cur_dir_node->total - 1))
							cur_dir_node->file_sel = cur_dir_node->file_sel + FILE_NUM_PERPAGE;
						else {
							if (cur_dir_node->file_sel < ((cur_dir_node->total - 1)/FILE_NUM_PERPAGE*FILE_NUM_PERPAGE))
								cur_dir_node->file_sel = (cur_dir_node->total - 1)/FILE_NUM_PERPAGE*FILE_NUM_PERPAGE;
							else
								cur_dir_node->file_sel = (cur_dir_node->file_sel)%FILE_NUM_PERPAGE;
						}
	                }	
					else
						cur_dir_node->file_sel = (cur_dir_node->file_sel)%FILE_NUM_PERPAGE;
					file_list =0;
	                break;
	            case KEY_P2_FUNC:
				case KEY_Y_FUNC:
	            case KEY_FIRE_FUNC:
					if (cur_dir_node->total != 0)
	                enter_folder(cur_dir_node);
	                file_list =0;
	                break;
			}
		}
	}
	return 0;
}

static int onrender()
{
	int page;
	int i,list_y=0;
	char path[256]={0};
	struct file_node *file_node_temp;

	if(0 == file_list){
		gr_color(0xff,  0xff, 0xff,0xff);//rgb a
		drawfullscreen(&browser_bg_bmap);
		page = (cur_dir_node->total + FILE_NUM_PERPAGE - 1) / FILE_NUM_PERPAGE;
	    file_node_temp = cur_dir_node->file_node_list;
	    for (i = 0; i < (cur_dir_node->file_sel / FILE_NUM_PERPAGE) * FILE_NUM_PERPAGE; i++) {
	        if (file_node_temp->next_node)
	            file_node_temp = file_node_temp->next_node;
	    }
		if(filter_type == FILTER_FILE_GAME && (cur_dir_node->total != 0)) {
			for (i = 0; i < FILE_NUM_PERPAGE; i++) {
	            if (((cur_dir_node->file_sel / FILE_NUM_PERPAGE) * FILE_NUM_PERPAGE + i) >= cur_dir_node->total)
	                break;
				if (i == (cur_dir_node->file_sel % FILE_NUM_PERPAGE)){
					drawrect_gamelist(&list_bmap,0,60+30*i);
					//printf("=======path= %s i:%d = %d; %d,%d=======\n",path,i,cur_dir_node->file_sel % FILE_NUM_PERPAGE,list_bmap.x_offs,list_bmap.y_offs);
				}
				draw_text(file_node_temp->name,50, 60+30*i,0 );
				//printf("=======browser name= %s =======\n",file_node_temp->name);
	            if (file_node_temp->next_node)
	                file_node_temp = file_node_temp->next_node;
	            else
	                break;
	        }
		}
		file_list = 1;
	}
	
	return 0;
}

struct page browser_page =
{
	"browser",
	init,
	deinit,
	onkey,
	onrender,
};
