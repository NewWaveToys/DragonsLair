# sub Makefile
CFLAGS +=  -I../include \
	#-DGLEW_EGL -DGLEW_NO_GLU -DMESA_EGL_NO_X11_HEADERS -mfpu=neon -marm -mfloat-abi=hard -mfpu=neon -marm -mfloat-abi=hard

%.d : %.cpp
	set -e; $(CXX) -MM $(CFLAGS) $< \
		| sed 's^\($*\)\.o[ :]*^\1.o $@ : ^g' > $@; \
		[ -s $@ ] || rm -f $@

#OBJS = game.o boardinfo.o lair.o superd.o thayers.o astron.o firefox.o \
	cliff.o speedtest.o seektest.o cputest.o ffr.o esh.o laireuro.o \
	badlands.o starrider.o bega.o multicputest.o cobraconv.o gpworld.o \
        interstellar.o benchmark.o lair2.o mach3.o lgp.o timetrav.o \
	releasetest.o singe.o test_sb.o
	
OBJS = game.o boardinfo.o lair.o thayers.o 

.SUFFIXES:	.cpp

all:	${OBJS}

include $(OBJS:.o=.d)

.cpp.o:
	${CXX} ${CFLAGS} -c $< -o $@

clean:
	rm ${OBJS} *.d
