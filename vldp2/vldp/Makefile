# sub Makefile
#CC = /home/developer/luwenjiang/3128linux/buildroot/output/rockchip_rk3128_game/host/bin/arm-buildroot-linux-gnueabihf-gcc
#AR = /home/developer/luwenjiang/3128linux/buildroot/output/rockchip_rk3128_game/host/bin/arm-buildroot-linux-gnueabihf-ar

CFLAGS += -fPIC \
	-I../include -I../../sdl_mods/sdl-1.2.15/include

%.d : %.c
	set -e; $(CC) -MM $(CFLAGS) $< \
		| sed 's^\($*\)\.o[ :]*^\1.o $@ : ^g' > $@; \
		[ -s $@ ] || rm -f $@


OBJS = vldp_internal.o vldp.o mpegscan.o 

.SUFFIXES:	.c

all:	${OBJS} 
	$(AR) cru ../../lib/libvldp.a ${OBJS}

include $(OBJS:.o=.d)

.c.o:
	${CC} ${CFLAGS} -c $< -o $@


clean:
	rm -f ${OBJS} *.d
	rm -f ../../lib/libvldp.a
