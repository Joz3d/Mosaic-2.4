# You shouldn't need to edit below here.

LIBTARGET = libXmx.a

all: $(LIBTARGET)

CFILES = Xmx.c Xmx2.c

OBJS = $(CFILES:.c=.o)

$(LIBTARGET): $(OBJS)
	-rm -f $(LIBTARGET)
	ar rv $(LIBTARGET) $(OBJS)
	$(RANLIB) $(LIBTARGET)

clean:
	-rm $(LIBTARGET) *.o
tags:
	etags -t *.[ch]
