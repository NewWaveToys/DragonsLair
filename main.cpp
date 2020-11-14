#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h> 
#include <sys/types.h>
#include <unistd.h>
#include <math.h>

typedef signed char int8_t;
typedef unsigned char uint8_t;
typedef signed short int16_t;
typedef unsigned short uint16_t;
typedef signed int int32_t;
typedef unsigned int uint32_t;
typedef signed long long int64_t;
typedef unsigned long long uint64_t;
#ifndef _SIZE_T_DEFINED_
#define _SIZE_T_DEFINED_
typedef unsigned int size_t;
#endif
typedef unsigned int uintptr_t;

typedef int8_t		Sint8;
typedef uint8_t		Uint8;
typedef int16_t		Sint16;
typedef uint16_t	Uint16;
typedef int32_t		Sint32;
typedef uint32_t	Uint32;

#include "ldp-out/ldp-vldp.h"
#include "game/lair.h"
#include "io/homedir.h"
#include "ldp-out/ldp.h"
#include "game/game.h"
#include "video/led.h"
#include "video/video.h"
#include "globals.h"

void set_quitflag()
{printf("%s==  %d \n",__FUNCTION__,__LINE__);
	quitflag = 1;
}

unsigned char get_quitflag()
{
	return(quitflag);
}
void set_idleexit(unsigned int value)
{printf("%s==  %d \n",__FUNCTION__,__LINE__);
	idleexit = value;
}

unsigned int get_idleexit()
{
	return(idleexit);
}
void set_scoreboard(unsigned char value)
{printf("%s==  %d \n",__FUNCTION__,__LINE__);
	realscoreboard = value;
}

unsigned char get_scoreboard()
{
	return (realscoreboard);
}
void set_scoreboard_port(unsigned int value)
{printf("%s==  %d \n",__FUNCTION__,__LINE__);
	rsb_port = value;
}
unsigned int get_scoreboard_port()
{
	return(rsb_port);
}
void set_search_offset(int i)
{printf("%s==  %d \n",__FUNCTION__,__LINE__);
	search_offset = i;
}

int get_search_offset()
{
	return(search_offset);
}

unsigned char get_frame_modifier()
{
	return(frame_modifier);
}

void set_frame_modifier(unsigned char value)
{printf("%s==  %d \n",__FUNCTION__,__LINE__);
	frame_modifier = value;
}
void set_serial_port(unsigned char i)
{printf("%s==  %d \n",__FUNCTION__,__LINE__);
	serial_port = i;
}


unsigned char get_serial_port()
{
	return(serial_port);
}

void set_baud_rate(int i)
{
	baud_rate = i;
}

int get_baud_rate()
{
	return(baud_rate);
}
void set_startsilent(unsigned char value)
{
	startsilent = value;
}

unsigned char get_startsilent()
{
	return(startsilent);
}
const char *get_daphne_version()
{
	return "1.0.7";
}
int main(int args, const char* argv[])
{
	printf("===0======\n");
	g_homedir.set_homedir("/mnt/sdcard");printf("====1=====\n");
	//chdir("/mnt/sdcard");
	SDL_putenv("homedir=/mnt/sdcard");
	SDL_putenv("SDL_VIDEODRIVER=dummy");
	g_game = new lair();printf("===2======\n");
	g_ldp = new ldp_vldp();printf("====3=====\n");
	ldp_vldp *cur_ldp = dynamic_cast<ldp_vldp *>(g_ldp);	// see if the currently selected LDP is VLDP
			
		if (cur_ldp)
		{
			cur_ldp->set_framefile("/mnt/sdcard/lair.txt");
		}
		else
		{
			printf("You can only set a framefile using VLDP as your laserdisc player!");
			
		}
		set_use_opengl(true);
		
			printf("=====4====\n");
			remember_leds();
			change_led(false, false, false);
			if (load_bmps() && init_display())
		{
			if (sound_init())
			{
				if (SDL_input_init())
				{
					// if the roms were loaded successfully
					if (g_game->load_roms())
					{
						// if the video was initialized successfully
						if (g_game->video_init())
						{
							// if the game has some problems, notify the user before the user plays the game
							if (g_game->get_issues())
							{
								printf(g_game->get_issues());
							}
printf("Booting ROM 000 \n");
							SDL_Delay(1000);
							// delay for a bit before the LDP is intialized to make sure
							// all video is done getting drawn before VLDP is initialized
printf("Booting ROM  == \n");
							// if the laserdisc player was initialized properly
							if (g_ldp->pre_init())
							{printf("Booting ROM ---\n");
								if (g_game->pre_init())     // initialize all cpu's
								{                         
									printf("Booting ROM ...\n");
									g_game->start();	// HERE IS THE MAIN LOOP RIGHT HERE
									g_game->pre_shutdown();

									// Send our game/ldp type to server to create stats.
									// This was moved to after the game has finished running because it creates a slight delay (like 1 second),
									//  which throws off the think_delay function in the LDP class.
									//net_send_data_to_server();

									//result_code = 0;	// daphne will exit without any errors
								}
								else
								{
									//exit if returns an error but don't print error message to avoid repetition
								}
								g_ldp->pre_shutdown();
							}
							else
							{
								printf("Could not initialize laserdisc player!");
							}
							g_game->video_shutdown();
						} // end if game video was initted properly
						else
						{
							printf("Game-specific video initialization failed!");
						}
					} // end if roms were loaded properly
					else
					{
						printf("Could not load ROM images! You must supply these.");
					}
					SDL_input_shutdown();
				}
				else
				{
					printf("Could not initialize input!");
				}
				sound_shutdown();
			}
			else
			{
				printf("Sound initialization failed!");
			}
			shutdown_display();	// shut down the display
		} // end init display
		else
		{
			printf("Video initialization failed!");
		}
	printf("=========\n");
	return 0;
}