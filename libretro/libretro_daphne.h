#pragma once

#ifndef LIBRETRO_DAPHNE_H__
#define LIBRETRO_DAPHNE_H__

/**************************************************************************************************
***************************************************************************************************
*
* Copyright (C) 2016 Seismal
*
* This file is part of the Libretro Daphne Core.
*
* Please see the LISCENSE file in the root of this project for distribution and
* modification terms.
*
* 2016.09.20 - RJS - Used for shim routines from Daphne to Libretro.
*
***************************************************************************************************
**************************************************************************************************/

#include <limits.h>

/**************************************************************************************************
***************************************************************************************************
*
* Structs, Defines, enums, etc.
*
***************************************************************************************************
**************************************************************************************************/
#define UNUSED(x) (void)(x)

#define DAPHNE_ROM_EXTENSION "zip"
#define DAPHNE_MAX_ROMNAME 15

#define DAPHNE_VIDEO_W		640
#define DAPHNE_VIDEO_H		480
#define DAPHNE_VIDEO_Wf		640.0f
#define DAPHNE_VIDEO_Hf		480.0f
#define DAPHNE_VIDEO_BPP	16
#define DAPHNE_VIDEO_ByPP	(DAPHNE_VIDEO_BPP / 8)
#define DAPHNE_TIMING_FPS	60.0f	
#define DAPHNE_AUDIO_SAMPLE_RATE	44100

// List of games used in Daphne.  This list isn't the official enum but should be.  List is used in
// this order in other places.
// Build out the command line v01.
/*
typedef enum DAPHNE_GAME_TYPE
{
	DAPHNE_GAME_TYPE_UNDEFINED,
	DAPHNE_GAME_TYPE_LAIR,
	DAPHNE_GAME_TYPE_LAIR2,
	DAPHNE_GAME_TYPE_ACE,
	DAPHNE_GAME_TYPE_ACE91,
	DAPHNE_GAME_TYPE_CLIFF,
	DAPHNE_GAME_TYPE_GTG,
	DAPHNE_GAME_TYPE_SUPERD,
	DAPHNE_GAME_TYPE_THAYERS,
	DAPHNE_GAME_TYPE_ASTRON,
	DAPHNE_GAME_TYPE_GALAXY,
	DAPHNE_GAME_TYPE_ESH,
	DAPHNE_GAME_TYPE_LAIREURO,
	DAPHNE_GAME_TYPE_BADLANDS,
	DAPHNE_GAME_TYPE_STARRIDER,
	DAPHNE_GAME_TYPE_BEGA,
	DAPHNE_GAME_TYPE_INTERSTELLAR,
	DAPHNE_GAME_TYPE_SAE,
	DAPHNE_GAME_TYPE_MACH3,
	DAPHNE_GAME_TYPE_UVT,
	DAPHNE_GAME_TYPE_BADLANDP,
	DAPHNE_GAME_TYPE_DLE1,
	DAPHNE_GAME_TYPE_DLE2,
	DAPHNE_GAME_TYPE_GPWORLD,
	DAPHNE_GAME_SUBTYPE_ASTRON_COBRA,
	DAPHNE_GAME_SUBTYPE_BEGA_ROADBLASTER,
	DAPHNE_GAME_TYPE_MAX
} DAPHNE_GAME_TYPE;
*/

/**************************************************************************************************
***************************************************************************************************
*
* Globals.
*
***************************************************************************************************
**************************************************************************************************/
// File and path of loaded rom.
extern char gstr_rom_path[1024];

extern char gstr_rom_name[DAPHNE_MAX_ROMNAME];
extern char gstr_rom_extension[sizeof(DAPHNE_ROM_EXTENSION)];

struct parameter_data
{
        unsigned version;
        unsigned language;
        unsigned screen_brightness;
        unsigned game_audio;
        unsigned attract_mode_audio;
        unsigned diagnostics;
        unsigned dirks_per_credit;
        unsigned marquee;
        unsigned difficulty_level;
        unsigned joystick_sound;
        unsigned screen_blanking;
        char m_switchA;
        char m_switchB;
};
extern struct parameter_data para_data;
extern int parameter_read(void);
 
/**************************************************************************************************
***************************************************************************************************
*
* Routines.
*
***************************************************************************************************
**************************************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif

   void retro_log(int in_debug_info_warn_error, const char *in_fmt, ...);

#ifdef __cplusplus
}
#endif

#endif
