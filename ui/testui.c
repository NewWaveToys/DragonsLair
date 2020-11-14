#include "minui/minui.h"
#include <string>

GRSurface* surf_unknown;
static int char_width;
static int char_height;
void healthd_board_mode_charger_set_backlight(bool en);

static int draw_text(const char *str, int x, int y)
{
    int str_len_px = gr_measure(gr_sys_font(), str);

    if (x < 0)
        x = (gr_fb_width() - str_len_px) / 2;
    if (y < 0)
        y = (gr_fb_height() - char_height) / 2;
    gr_text(gr_sys_font(), x, y, str, 0);

    return y + char_height;
}

static void android_green(void)
{
    gr_color(0xa4, 0xc6, 0x39, 255);
}

static void clear_screen(void)
{
    gr_color(0, 0, 0, 255);
    gr_clear();
}

/* returns the last y-offset of where the surface ends */
static int draw_surface_centered(GRSurface* surface)
{
    int w;
    int h;
    int x;
    int y;

    w = gr_get_width(surface);
    h = gr_get_height(surface);
    x = (gr_fb_width() - w) / 2 ;
    y = (gr_fb_height() - h) / 2 ;

    printf("drawing surface %dx%d+%d+%d\n", w, h, x, y);
    gr_blit(surface, 0, 0, w, h, x, y);
    return y + h;
}

static void draw_picture(void)
{
	int ret = -1;
    std::string animation_file;
printf("%s ==%d \n",__FILE__,__LINE__);
	animation_file.assign("g");
	ret = res_create_display_surface(animation_file.c_str(), &surf_unknown);
	printf("%s ==%d \n",__FILE__,__LINE__);
	if(ret < 0)
	{
		printf("surface %s create failed\n",animation_file.c_str());
	}
	draw_surface_centered(surf_unknown);
	
}
int main(void)
{printf("%s ==%d \n",__FILE__,__LINE__);
	gr_init();printf("%s ==%d \n",__FILE__,__LINE__);
	gr_font_size(gr_sys_font(), &char_width, &char_height);
	printf("%s ==%d \n",__FILE__,__LINE__);
	clear_screen();
	//healthd_board_mode_charger_set_backlight(true);
printf("%s ==%d \n",__FILE__,__LINE__);
	
	android_green();printf("%s ==%d \n",__FILE__,__LINE__);
    //draw_text("Hello World!", -1, -1);
    draw_picture();printf("%s ==%d \n",__FILE__,__LINE__);
	gr_flip();
	
	printf("run anim\n");

	while(1);
	return 0;
}
