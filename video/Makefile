# sub Makefile
CFLAGS +=   \
	#-DGLEW_EGL -DGLEW_NO_GLU -DMESA_EGL_NO_X11_HEADERS -mfpu=neon -marm -mfloat-abi=hard -mfpu=neon -marm -mfloat-abi=hard
%.d : %.cpp
	set -e; $(CXX) -MM $(CFLAGS) $< \
		| sed 's^\($*\)\.o[ :]*^\1.o $@ : ^g' > $@; \
		[ -s $@ ] || rm -f $@

OBJS = video.o  blend.o rgb2yuv.o \
	#SDL_Console.o SDL_DrawText.o \
	SDL_ConsoleCommands.o led.o palette.o tms9128nl.o 
	

.SUFFIXES:	.cpp

all:	${OBJS} rgb2yuv-asm blend_mmx-asm

include $(OBJS:.o=.d)

.cpp.o:
	${CXX} ${CFLAGS} -c $< -o $@

# this assembly language code is MMX only, so we have a conditional here
# so that non-MMX cpu's don't choke here ...
rgb2yuv-asm:	rgb2yuv-gas.s
ifeq ($(USE_MMX),1)
	$(AS) rgb2yuv-gas.s -gnu -o rgb2yuv-gas.o
endif

blend_mmx-asm:	blend_mmx-gas.s
ifeq ($(USE_MMX),1)
	$(AS) blend_mmx-gas.s -o blend_mmx-gas.o
endif

clean:
	rm -f ${OBJS} *.d
