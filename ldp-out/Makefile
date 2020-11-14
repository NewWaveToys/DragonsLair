# sub Makefile
CFLAGS += -I../libvorbis-1.3.6/include -I../libogg-1.3.3/include -I../include \
#	-DGLEW_EGL -DGLEW_NO_GLU -DMESA_EGL_NO_X11_HEADERS -mfpu=neon -marm -mfloat-abi=hard -mfpu=neon -marm -mfloat-abi=hard

%.d : %.cpp
	set -e; $(CXX) -MM $(CFLAGS) $< \
		| sed 's^\($*\)\.o[ :]*^\1.o $@ : ^g' > $@; \
		[ -s $@ ] || rm -f $@

OBJS = sony.o pioneer.o ld-v6000.o hitachi.o philips.o ldp.o framemod.o \
	ldp-combo.o ldp-vldp.o ldp-vldp-audio.o
#	ldp-vldp-gl.o ldp-vldp-gp2x.o

.SUFFIXES:	.cpp

all:	${OBJS}

include $(OBJS:.o=.d)

.cpp.o:
	${CXX} ${CFLAGS} -c $< -o $@

clean:
	rm -f ${OBJS} *.d
