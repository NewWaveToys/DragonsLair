#include <stdio.h>
#include "interface.h"
#include "fbviewer.h"
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <errno.h>

#include <time.h>
#include "rthreads/rthreads.h"


static char *model = 0;
static char *data_code = 0;
static char *system_version = 0;
static char *pcba_version = 0;
static char *verstion = 0;
static char *checksun = 0;

static char *model_disp = 0;
static char *data_code_disp = 0;
static char *system_version_disp = 0;
static char *pcba_version_disp = 0;
static char *verstion_disp = 0;
static char *checksun_disp = 0;

static char testmdoe_change = 0;

#define maxlabelsize 35
static int  __getline(char **lineptr, ssize_t *n, FILE *stream)
{
    int count=0;
    int buf;

    if(*lineptr == NULL) {
        *n=maxlabelsize;
        *lineptr = (char*)malloc(*n);
    }

    if(( buf = fgetc(stream) ) == EOF ) {
        return -1;
    }

    do {
        if(buf=='\n') {
            count += 1;
            break;
        }

        count++;

        *(*lineptr+count-1) = buf;
        *(*lineptr+count) = '\0';

        if(*n <= count)
            *lineptr = (char*)realloc(*lineptr,count*2);
        buf = fgetc(stream);
    } while( buf != EOF);

    return count;
}

int loadversion(char **model, char **data_c, char **sys_version, char **pcb_version, char **version, char **checksun)
{
    FILE *fp;
    ssize_t len = 0;
    int pos = 0;
    const char* versionFile;

    versionFile = VERSION_FILE;

    fp = fopen(versionFile, "r");
    if (fp == NULL) {
        printf("open file %s failed: %s\n", versionFile, strerror(errno));
        return -1;
    }

    if ((__getline(model, &len, fp)) == -1)
        return -1;

        if ((__getline(data_c, &len, fp)) == -1)
        return -1;

        if ((__getline(sys_version, &len, fp)) == -1)
        return -1;

    if ((__getline(pcb_version, &len, fp)) == -1)
        return -1;

    if ((__getline(version, &len, fp)) == -1)
        return -1;

        if ((__getline(checksun, &len, fp)) == -1)
        return -1;
    fclose(fp);

    return 0;
}

static int loadres_sysinformation(void)
{
    int len;
    char *checksun_str;
    loadversion(&model, &data_code, &system_version, &pcba_version, &verstion, &checksun);
    if (model) {
        len = strlen(model) + 1;
        model_disp = malloc(len);
        snprintf(model_disp, len, "%s", model);
        free(model);
        model = 0;
    }
    if (data_code) {
        len = strlen(data_code) + 1;
        data_code_disp = malloc(len);
        snprintf(data_code_disp, len, "%s", data_code);
        free(data_code);
        data_code = 0;
    }
    if (system_version) {
        len = strlen(system_version) + 1;
        system_version_disp = malloc(len);
        snprintf(system_version_disp, len, "%s", system_version);
        free(system_version);
        system_version = 0;
    }
    if (pcba_version) {
        len = strlen(pcba_version) + 1;
        pcba_version_disp = malloc(len);
        snprintf(pcba_version_disp, len, "%s", pcba_version);
        free(pcba_version);
        pcba_version = 0;
    }
    if (verstion) {
        len = strlen(verstion)+ 1;
        verstion_disp = malloc(len);
        snprintf(verstion_disp, len, "%s", verstion);
        free(verstion);
        verstion = 0;
    }
    if (checksun) {
        checksun_str=strtok(checksun," ");
        len = strlen(checksun_str) + 1;
        checksun_disp = malloc(len);
        snprintf(checksun_disp, len, "%s", checksun);
        free(checksun);
        checksun = 0;
    }
    return 0;
}

static void unloadres_sysinformation(void)
{
    if (model_disp)
        free(model_disp);
    model_disp = 0;

    if (data_code_disp)
        free(data_code_disp);
    data_code_disp = 0;

    if (system_version_disp)
        free(system_version_disp);
    system_version_disp = 0;

    if (pcba_version_disp)
        free(pcba_version_disp);
    pcba_version_disp = 0;

    if (verstion_disp)
        free(verstion_disp);
    verstion_disp = 0;

    if (checksun_disp)
    	free(checksun_disp);
    checksun_disp = 0;
}

static int init(const char* dir)
{
	printf("=====init testmode======\n");
	loadres_sysinformation();
	testmdoe_change = 1;
	return 0;
}

static int deinit()
{
	unloadres_sysinformation();
	return 0;
}

static int onkey(unsigned short code, unsigned int value)
{
	if(value == 1) {
		if((code == KEY_P2_FUNC) || (code == KEY_Y_FUNC))
			return 1;
	}
	return 0;
}

static int onrender()
{
	if(testmdoe_change == 1) {
		drawcolor(0xff000000);
		gr_color(0xff,  0xff, 0xff,0xff);
		draw_text(model_disp,-1,270,1);
		draw_text(data_code_disp,-1,270+30,1);
		draw_text(system_version_disp,-1,270+60,1);
		draw_text(pcba_version_disp,-1,270+90,1);
		draw_text(verstion_disp,-1,270+120,1);
		draw_text(checksun_disp,-1,270+150,1);
		testmdoe_change = 0;
	}
	return 0;
}

struct page testmode_page =
{
	"testmode",
	init,
	deinit,
	onkey,
	onrender,
};
