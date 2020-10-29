# Makefile for DAPHNE
# Written by Matt Ownby

# You need to symlink the set of variables that correspond to that platform
# that you are using.  For example, if you are compiling for linux, you would
# do "ln -s Makefile.vars.linux Makefile.vars"
include Makefile.vars

# send these to all the sub-Makefiles

# name of the executable
EXE = daphne.bin
LIBRETRO=1
VLDP_OBJS =

# if we are statically linking VLDP (instead of dynamic)
# NOTE : these libs must be compiled separately beforehand (as it building a shared vldp)
ifeq ($(STATIC_VLDP),1)
VLDP_OBJS = vldp2/vldp/vldp.o vldp2/vldp/vldp_internal.o vldp2/vldp/mpegscan.o \
	vldp2/libmpeg2/cpu_accel.o vldp2/libmpeg2/alloc.o vldp2/libmpeg2/cpu_state.o vldp2/libmpeg2/decode.o \
	vldp2/libmpeg2/header.o vldp2/libmpeg2/motion_comp.o vldp2/libmpeg2/idct.o vldp2/libmpeg2/idct_mmx.o \
	vldp2/libmpeg2/motion_comp_mmx.o vldp2/libmpeg2/slice.o vldp2/libvo/video_out.o vldp2/libvo/video_out_null.o
DEFINE_STATIC_VLDP = -DSTATIC_VLDP
endif

# gp2x static linking is slightly different because the decoding
#  is done on the 940 cpu
ifeq ($(STATIC_VLDP_GP2X),1)
VLDP_OBJS = vldp2/vldp/vldp.o vldp2/vldp/vldp_internal.o vldp2/vldp/mpegscan.o \
	vldp2/libvo/video_out_null.o vldp2/940/interface_920.o
EXE = ../daphne2x
DEFINE_STATIC_VLDP = -DSTATIC_VLDP
endif

# Platform specific cflags defined in the Makefile.vars file
export CFLAGS = ${PFLAGS} ${DEFINE_STATIC_VLDP} -Wall -I../ -I../include -I../glew-2.1.0/include \
		-I/home/developer/luwenjiang/3128linux/buildroot/output/rockchip_rk3128_game/staging/usr/include/libdrm \
		-I../sdl_mods/sdl-1.2.15/include \
		-I../../sdl_mods/sdl-1.2.15/include -Isdl_mods/sdl-1.2.15/include \
#		-I/home/developer/luwenjiang/3128linux/buildroot/output/rockchip_rk3128_game/staging/usr/include/SDL

#OBJS = ldp-out/*.o cpu/*.o game/*.o io/*.o timer/*.o ldp-in/*.o video/*.o \
	sound/*.o daphne.o  scoreboard/*.o ${VLDP_OBJS}
	
#SDL_OBJS = sdl_mods/sdl-1.2.15/src/SDL.o sdl_mods/sdl-1.2.15/src/SDL_error.o sdl_mods/sdl-1.2.15/src/SDL_fatal.o \
sdl_mods/sdl-1.2.15/src/audio/SDL_audio.o sdl_mods/sdl-1.2.15/src/audio/SDL_audiocvt.o sdl_mods/sdl-1.2.15/src/audio/SDL_audiodev.o sdl_mods/sdl-1.2.15/src/audio/SDL_mixer.o sdl_mods/sdl-1.2.15/src/audio/SDL_mixer_MMX.o \
sdl_mods/sdl-1.2.15/src/audio/SDL_mixer_MMX_VC.o sdl_mods/sdl-1.2.15/src/audio/SDL_mixer_m68k.o sdl_mods/sdl-1.2.15/src/audio/SDL_wave.o \
sdl_mods/sdl-1.2.15/src/cdrom/SDL_cdrom.o sdl_mods/sdl-1.2.15/src/cpuinfo/SDL_cpuinfo.o sdl_mods/sdl-1.2.15/src/events/SDL_active.o sdl_mods/sdl-1.2.15/src/events/SDL_events.o sdl_mods/sdl-1.2.15/src/events/SDL_expose.o \
sdl_mods/sdl-1.2.15/src/events/SDL_keyboard.o sdl_mods/sdl-1.2.15/src/events/SDL_mouse.o sdl_mods/sdl-1.2.15/src/events/SDL_quit.o sdl_mods/sdl-1.2.15/src/events/SDL_resize.o \
sdl_mods/sdl-1.2.15/src/file/SDL_rwops.o sdl_mods/sdl-1.2.15/src/stdlib/SDL_getenv.o sdl_mods/sdl-1.2.15/src/stdlib/SDL_iconv.o sdl_mods/sdl-1.2.15/src/stdlib/SDL_malloc.o \
sdl_mods/sdl-1.2.15/src/stdlib/SDL_qsort.o sdl_mods/sdl-1.2.15/src/stdlib/SDL_stdlib.o sdl_mods/sdl-1.2.15/src/stdlib/SDL_string.o \
sdl_mods/sdl-1.2.15/src/thread/SDL_thread.o sdl_mods/sdl-1.2.15/src/timer/SDL_timer.o sdl_mods/sdl-1.2.15/src/video/SDL_RLEaccel.o sdl_mods/sdl-1.2.15/src/video/SDL_blit.o sdl_mods/sdl-1.2.15/src/video/SDL_blit_0.o \
sdl_mods/sdl-1.2.15/src/video/SDL_blit_1.o sdl_mods/sdl-1.2.15/src/video/SDL_blit_A.o sdl_mods/sdl-1.2.15/src/video/SDL_blit_N.o sdl_mods/sdl-1.2.15/src/video/SDL_bmp.o sdl_mods/sdl-1.2.15/src/video/SDL_cursor.o \
sdl_mods/sdl-1.2.15/src/video/SDL_gamma.o sdl_mods/sdl-1.2.15/src/video/SDL_pixels.o sdl_mods/sdl-1.2.15/src/video/SDL_stretch.o \
sdl_mods/sdl-1.2.15/src/video/SDL_surface.o sdl_mods/sdl-1.2.15/src/video/SDL_video.o sdl_mods/sdl-1.2.15/src/video/SDL_yuv.o sdl_mods/sdl-1.2.15/src/video/SDL_yuv_mmx.o sdl_mods/sdl-1.2.15/src/video/SDL_yuv_sw.o \
sdl_mods/sdl-1.2.15/src/joystick/SDL_joystick.o sdl_mods/sdl-1.2.15/src/video/dummy/SDL_nullevents.o sdl_mods/sdl-1.2.15/src/video/dummy/SDL_nullmouse.o \
sdl_mods/sdl-1.2.15/src/video/dummy/SDL_nullvideo.o sdl_mods/sdl-1.2.15/src/audio/disk/SDL_diskaudio.o sdl_mods/sdl-1.2.15/src/audio/dummy/SDL_dummyaudio.o sdl_mods/sdl-1.2.15/src/loadso/dlopen/SDL_sysloadso.o sdl_mods/sdl-1.2.15/src/audio/dsp/SDL_dspaudio.o sdl_mods/sdl-1.2.15/src/audio/dma/SDL_dmaaudio.o sdl_mods/sdl-1.2.15/src/audio/alsa/SDL_alsa_audio.o \
sdl_mods/sdl-1.2.15/src/thread/pthread/SDL_systhread.o sdl_mods/sdl-1.2.15/src/thread/pthread/SDL_syssem.o sdl_mods/sdl-1.2.15/src/thread/pthread/SDL_sysmutex.o sdl_mods/sdl-1.2.15/src/thread/pthread/SDL_syscond.o \
sdl_mods/sdl-1.2.15/src/joystick/linux/SDL_sysjoystick.o sdl_mods/sdl-1.2.15/src/cdrom/linux/SDL_syscdrom.o sdl_mods/sdl-1.2.15/src/timer/unix/SDL_systimer.o	
	
OBJS = ldp-out/*.o cpu/*.o cpu/x86/*.o game/*.o io/*.o timer/*.o ldp-in/*.o video/*.o sound/*.o\
		scoreboard/*.o  libogg-1.3.4/src/*.o libvorbis-1.3.7/lib/*.o  \
#				glew-2.1.0/src/*.o
#${VLDP_OBJS} ${SDL_OBJS}

LOCAL_OBJS = daphne.o

ifeq ($(LIBRETRO),1)
 fpic := -fPIC
SHARED := -shared -Wl,--version-script=link.T -Wl,--no-undefined
EXE = libdaphne.so
CFLAGS +=-DLIBRETRO=1 -Ivldp2/include  -D_GNU_SOURCE=1 -D_REENTRANT \
	-Iui
UI_OBJS = ${VLDP_OBJS}  
endif
.SUFFIXES:	.cpp

all:	$(UI_OBJS) ${LOCAL_OBJS} sub
	${CXX} $(fpic) $(SHARED) ${DFLAGS} ${OBJS} ${LOCAL_OBJS} $(UI_OBJS) -o ${EXE} ${LIBS}

sub:
	cd ldp-out && $(MAKE)
	cd cpu && $(MAKE)
	cd cpu/x86 && $(MAKE)
	cd game && $(MAKE)
	cd io && $(MAKE)
	cd timer && $(MAKE)
	cd ldp-in && $(MAKE)
	cd video && $(MAKE)
	cd sound && $(MAKE)
	cd scoreboard && $(MAKE)
	cd libogg-1.3.4 && $(MAKE) 
	cd libvorbis-1.3.7 && $(MAKE) 
	cd sdl_mods/sdl-1.2.15 && $(MAKE) 
	cd vldp2/vldp && $(MAKE) 
	cd glew-2.1.0 && $(MAKE)

include $(LOCAL_OBJS:.o=.d)

.cpp.o:
	${CXX} ${CFLAGS} -c $< -o $@
.c.o:
	${CC} ${CFLAGS} -c $< -o $@
	
clean_deps:
	find . -name "*.d" -exec rm {} \;

clean:	clean_deps
	find . -name "*.o" -exec rm {} \;
	rm -f ${EXE}

testvldp:	testvldp.c
	${CC} ${CFLAGS} -DSHOW_FRAMES -DUSE_OVERLAY testvldp.c ${VLDP_OBJS} ${LIBS} -o ../testvldp

%.d : %.cpp
	set -e; $(CXX) -MM $(CFLAGS) $< \
                | sed 's^\($*\)\.o[ :]*^\1.o $@ : ^g' > $@; \
                [ -s $@ ] || rm -f $@
