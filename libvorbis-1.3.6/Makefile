# sub Makefile

CFLAGS += -Iinclude/ -I../libogg-1.3.3/include/ -fPIC \

%.d : %.c
	set -e; $(CC) -MM $(CFLAGS) $< \
		| sed 's^\($*\)\.o[ :]*^\1.o $@ : ^g' > $@; \
		[ -s $@ ] || rm -f $@

OBJS = lib/vorbisfile.o lib/mdct.o lib/smallft.o lib/block.o lib/envelope.o lib/window.o lib/lsp.o \
		lib/lpc.o lib/analysis.o lib/synthesis.o lib/psy.o lib/info.o \
		lib/floor1.o lib/floor0.o\
		lib/res0.o lib/mapping0.o lib/registry.o lib/codebook.o lib/sharedbook.o\
		lib/lookup.o lib/bitrate.o\

.SUFFIXES:	.c

all:	${OBJS} 

include $(OBJS:.o=.d)

.c.o:
	${CC} ${CFLAGS} -c $< -o $@

# MMX audio mixing function (conditional upon whether USE_MMX is defined)
mix_mmx-asm:  mix_mmx-gas.s
ifeq ($(USE_MMX),1)
	as mix_mmx-gas.s -o mix_mmx-gas.o
endif

clean:
	rm -f ${OBJS} *.d
