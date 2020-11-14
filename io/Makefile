# sub Makefile

%.d : %.cpp
	set -e; $(CXX) -MM $(CFLAGS) $< \
		| sed 's^\($*\)\.o[ :]*^\1.o $@ : ^g' > $@; \
		[ -s $@ ] || rm -f $@

OBJS = input.o serial.o conout.o conin.o parallel.o error.o \
	 sram.o fileparse.o unzip.o mpo_fileio.o numstr.o homedir.o \
	logger.o logger_console.o logger_factory.o  

#	  network.o cmdline.o 
.SUFFIXES:	.cpp

all:	${OBJS}

include $(OBJS:.o=.d)

.cpp.o:
	${CXX} ${CFLAGS} -c $< -o $@

clean:
	rm -f ${OBJS} *.d
