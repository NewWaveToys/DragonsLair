# sub Makefile
CFLAGS += -Iinclude -DGLEW_NO_GLU -DGLEW_STATIC -DGLEW_EGL  -DMESA_EGL_NO_X11_HEADERS

%.d : %.c
	set -e; $(CC) -MM $(CFLAGS) $< \
		| sed 's^\($*\)\.o[ :]*^\1.o $@ : ^g' > $@; \
		[ -s $@ ] || rm -f $@

OBJS = src/glew.o 

.SUFFIXES:	.c

all:	${OBJS} 

include $(OBJS:.o=.d)

.c.o:
	${CC} ${CFLAGS} -c $< -o $@


clean:
	rm -f ${OBJS} *.d
