/*
 * Copyright (c) 2018 rockchip
 *
 */
#ifndef _PARAMETER_H_
#define _PARAMETER_H_
#if defined(__cplusplus) 
extern "C"{
#endif

int parameter_recovery(void);
int parameter_init(void);
void parameter_deinit(void);


//void set_no_keys_val(int val);
//int get_no_keys_val(void);
void set_screen_brightness(unsigned char val);
unsigned char get_screen_brightness(void);

void set_game_audio(unsigned char val);
unsigned char get_game_audio(void);

void set_attract_audio(unsigned char val);
unsigned char get_attract_audio(void);

void set_marquee(unsigned char val);
unsigned char get_marquee(void);

void set_dirks_per_credit(unsigned char val);
unsigned char get_dirks_per_credit(void);

void set_difficulty_level(unsigned char val);
unsigned char get_difficulty_level(void);

void set_joystick_sound(unsigned char val);
unsigned char get_joystick_sound(void);

void set_screen_blanking(unsigned char val);
unsigned char get_screen_blanking(void);

void reset_change();
char game_is_change();


char *get_ui_image_path(void);


#if defined(__cplusplus) 
}
#endif

#endif
