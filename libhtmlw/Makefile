LIBTARGET = libhtmlw.a

all: $(LIBTARGET)

CFILES = HTML.c HTMLlists.c HTMLformat.c HTMLparse.c HTMLimages.c HTMLwidgets.c HTML-PSform.c DrawingArea.c HTMLjot.c

OBJS = $(CFILES:.c=.o)

$(LIBTARGET): $(OBJS)
	-rm -f $(LIBTARGET)
	ar rv $(LIBTARGET) $(OBJS)
	$(RANLIB) $(LIBTARGET)

clean:
	-rm $(LIBTARGET) *.o
tags:
	etags -t *.[ch]
