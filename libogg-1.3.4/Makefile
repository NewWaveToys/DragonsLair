# sub Makefile
#CC = /home/developer/luwenjiang/3128linux/buildroot/output/rockchip_rk3128_game/host/bin/arm-buildroot-linux-gnueabihf-gcc
#AR = /home/developer/luwenjiang/3128linux/buildroot/output/rockchip_rk3128_game/host/bin/arm-buildroot-linux-gnueabihf-ar

CFLAGS += -Iinclude/

%.d : %.c
	set -e; $(CC) -MM $(CFLAGS) $< \
		| sed 's^\($*\)\.o[ :]*^\1.o $@ : ^g' > $@; \
		[ -s $@ ] || rm -f $@

OBJS = src/bitwise.o src/framing.o 

.SUFFIXES:	.c

all:	${OBJS} 

include $(OBJS:.o=.d)

.c.o:
	${CC} ${CFLAGS} -c $< -o $@

# MMX audio mixing function (conditional upon whether USE_MMX is defined)
mix_mmx-asm:  mix_mmx-gas.s
ifeq ($(USE_MMX),1)
	$(AS) mix_mmx-gas.s -o mix_mmx-gas.o
endif

clean:
	rm -f ${OBJS} *.d
