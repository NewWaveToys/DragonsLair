# sub Makefile
CFLAGS = -fPIC -Iinclude \
 -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64  \
 -D_GNU_SOURCE=1 -fvisibility=hidden  -D_REENTRANT -DHAVE_LINUX_VERSION_H
 
%.d : %.c
	set -e; $(CC) -MM $(CFLAGS) $< \
		| sed 's^\($*\)\.o[ :]*^\1.o $@ : ^g' > $@; \
		[ -s $@ ] || rm -f $@

OBJS = src/SDL.o src/SDL_error.o src/SDL_fatal.o \
src/audio/SDL_audio.o src/audio/SDL_audiocvt.o src/audio/SDL_audiodev.o src/audio/SDL_mixer.o src/audio/SDL_mixer_MMX.o \
src/audio/SDL_mixer_MMX_VC.o src/audio/SDL_mixer_m68k.o src/audio/SDL_wave.o \
src/cdrom/SDL_cdrom.o src/cpuinfo/SDL_cpuinfo.o src/events/SDL_active.o src/events/SDL_events.o src/events/SDL_expose.o \
src/events/SDL_keyboard.o src/events/SDL_mouse.o src/events/SDL_quit.o src/events/SDL_resize.o \
src/file/SDL_rwops.o src/stdlib/SDL_getenv.o src/stdlib/SDL_iconv.o src/stdlib/SDL_malloc.o \
src/stdlib/SDL_qsort.o src/stdlib/SDL_stdlib.o src/stdlib/SDL_string.o \
src/thread/SDL_thread.o src/timer/SDL_timer.o src/video/SDL_RLEaccel.o src/video/SDL_blit.o src/video/SDL_blit_0.o \
src/video/SDL_blit_1.o src/video/SDL_blit_A.o src/video/SDL_blit_N.o src/video/SDL_bmp.o src/video/SDL_cursor.o \
src/video/SDL_gamma.o src/video/SDL_pixels.o src/video/SDL_stretch.o \
src/video/SDL_surface.o src/video/SDL_video.o src/video/SDL_yuv.o src/video/SDL_yuv_mmx.o src/video/SDL_yuv_sw.o \
src/joystick/SDL_joystick.o src/video/dummy/SDL_nullevents.o src/video/dummy/SDL_nullmouse.o \
src/video/dummy/SDL_nullvideo.o src/audio/disk/SDL_diskaudio.o src/audio/dummy/SDL_dummyaudio.o src/loadso/dlopen/SDL_sysloadso.o src/audio/dsp/SDL_dspaudio.o src/audio/dma/SDL_dmaaudio.o src/audio/alsa/SDL_alsa_audio.o \
src/thread/pthread/SDL_systhread.o src/thread/pthread/SDL_syssem.o src/thread/pthread/SDL_sysmutex.o src/thread/pthread/SDL_syscond.o \
src/joystick/linux/SDL_sysjoystick.o src/cdrom/linux/SDL_syscdrom.o src/timer/unix/SDL_systimer.o


.SUFFIXES:	.c

all:	${OBJS} 
	$(AR) cru ../../lib/libsdl.a ${OBJS}

include $(OBJS:.o=.d)

.c.o:
	${CC} ${CFLAGS} -c $< -o $@


clean:
	rm -f ${OBJS} *.d
	rm -f ../../lib/libsdl.a