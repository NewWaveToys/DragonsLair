#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h> 
#include <sys/types.h>
#include <unistd.h>
#include <math.h>
#include <SDL.h>

int  SDL_InitSubSystem(unsigned int flags)
{
	return 0;
}
int  SDL_FillRect
		(SDL_Surface *dst, SDL_Rect *dstrect, unsigned int color)
{
	return 0;
}
void  SDL_UpdateRects
		(SDL_Surface *screen, int numrects, SDL_Rect *rects)
		{
}
int  SDL_UpperBlit
			(SDL_Surface *src, SDL_Rect *srcrect,
			 SDL_Surface *dst, SDL_Rect *dstrect)
{
	 return 0;
}
Uint32 SDL_MapRGB
(const SDL_PixelFormat * const format,
 const Uint8 r, const Uint8 g, const Uint8 b)
 {
	 return 0;
 }

 int SDL_Flip(SDL_Surface *screen)
 {
	 return 0;
 }
 SDL_Surface *  SDL_LoadBMP_RW(SDL_RWops *src, int freesrc)
 {
	 return NULL;
 }
 
 SDL_Surface *  SDL_CreateRGBSurface
			(Uint32 flags, int width, int height, int depth, 
			Uint32 Rmask, Uint32 Gmask, Uint32 Bmask, Uint32 Amask){
	return NULL;
}
SDL_Surface * SDLCALL SDL_SetVideoMode
			(int width, int height, int bpp, Uint32 flags)
{
	return NULL;
}
int SDL_ShowCursor(int toggle)
{
	return 0;
}
const SDL_VideoInfo * SDLCALL SDL_GetVideoInfo(void)
{
	return NULL;
}
int SDLCALL SDL_LockYUVOverlay(SDL_Overlay *overlay)
{
	return 0;
}
void SDLCALL SDL_UnlockYUVOverlay(SDL_Overlay *overlay)
{
	
}
void SDL_FreeYUVOverlay(SDL_Overlay *overlay)
{
}
SDL_Overlay * SDLCALL SDL_CreateYUVOverlay(int width, int height,
				Uint32 format, SDL_Surface *display)
{
	return NULL;
}
int SDLCALL SDL_DisplayYUVOverlay(SDL_Overlay *overlay, SDL_Rect *dstrect)
{
	return 0;
}
extern "C" void SDLDrawText(const char *string, SDL_Surface *surface, int FontType, int x, int y )
{
}
void SDL_WM_SetCaption(const char *title, const char *icon)
{
	
}
void SDL_FreeSurface(SDL_Surface *surface)
{
}
int SDL_ConvertAudio(SDL_AudioCVT *cvt)
{
	return 0;
}
int SDL_BuildAudioCVT(SDL_AudioCVT *cvt,
		Uint16 src_format, Uint8 src_channels, int src_rate,
		Uint16 dst_format, Uint8 dst_channels, int dst_rate)
{
	return 0;
}
 void SDL_FreeWAV(Uint8 *audio_buf)
 {
 }
 SDL_AudioSpec *  SDL_LoadWAV_RW(SDL_RWops *src, int freesrc, SDL_AudioSpec *spec, Uint8 **audio_buf, Uint32 *audio_len)
 {
	 return NULL;
 }
 void SDLCALL SDL_LockAudio(void)
 {
 }
 void SDLCALL SDL_UnlockAudio(void)
 {
 }
void SDL_PauseAudio(int pause_on)
{
}
int SDLCALL SDL_OpenAudio(SDL_AudioSpec *desired, SDL_AudioSpec *obtained)
{
	memcpy(obtained, desired, sizeof(SDL_AudioSpec));
	return 0;
}
void SDL_CloseAudio(void)
{
	
}	
void SDL_Delay(Uint32 ms)
{
}
Uint32  SDL_GetTicks(void)
{
}
int SDL_SaveBMP_RW
		(SDL_Surface *surface, SDL_RWops *dst, int freedst)
{
	return 0;
}
SDL_RWops *  SDL_RWFromFile(const char *file, const char *mode)
{
	return NULL;
}
void SDL_GL_SwapBuffers(void)
{
}
void SDL_QuitSubSystem(Uint32 flags)
{
}
char *  SDL_GetError(void)
{
	return NULL;
}
int SDLCALL SDL_mutexV(SDL_mutex *mutex)
{
	return 0;
}
int SDL_mutexP(SDL_mutex *mutex)
{
	return 0;
}
Uint8 SDLCALL SDL_EventState(Uint8 type, int state)
{
	return 0;
}
int SDL_NumJoysticks(void)
{
	return 0;
}
SDL_Joystick * SDLCALL SDL_JoystickOpen(int device_index)
{
	return NULL;
}
int SDL_EnableUNICODE(int enable)
{
	return 0;
}
int SDL_PollEvent(SDL_Event *event)
{
	return 0;
}

SDL_mutex * SDLCALL SDL_CreateMutex(void)
{
	return NULL;
}
void SDL_DestroyMutex(SDL_mutex *mutex)
{
	
}
//==========
extern "C" void DrawConsole()
{
}
extern "C" void ConsoleShutdown()
{
}
extern "C" void ConsoleEvents(SDL_Event *event)
{
}
extern "C" int ConsoleInit(const char *FontName, SDL_Surface *DisplayScreen, int lines)
{
	return 0;
}
extern "C" void AddCommand(void (*CommandCallback)(char *Parameters),const char *CommandWord)
{
}