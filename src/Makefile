all: Mosaic

LIBHTMLW_DIR = ../libhtmlw
LIBXMX_DIR = ../libXmx
LIBXMX_INC = ../libXmx

PROGRAM_LIBS = $(LIBWWW_DIR)/libwww.a $(LIBHTMLW_DIR)/libhtmlw.a $(LIBXMX_DIR)/libXmx.a
LIBS = $(PROGRAM_LIBS) $(X_LIBS) $(DTM_LIBS) $(HDF_LIBS) $(WAIS_LIBS) $(MATH_LIB) $(SYS_LIBS)

CFILES = main.c gui.c gui-dialogs.c gui-menubar.c gui-docume.c mo-www.c\
  mo-dtm.c hotlist.c whine.c history.c\
  annotate.c pan.c grpan.c grpan-www.c audan.c globalhist.c img.c\
  picread.c xpmhash.c xpmread.c gifread.c pixmaps.c\
  medcut.c mo-hdf.c
OBJS = main.o gui.o gui-dialogs.o gui-menubar.o gui-docume.o mo-www.o\
  mo-dtm.o hotlist.o whine.o history.o\
  annotate.o pan.o grpan.o grpan-www.o audan.o globalhist.o img.o\
  picread.o xpmhash.o xpmread.o gifread.o pixmaps.o\
  medcut.o mo-hdf.o

AUXCFILES = hdf-browse.c
AUXOBJS = hdf-browse.o

hdf-browse.o: hdf-browse.c ../libhtmlw/HTML.h
	$(CC) -c $(CFLAGS) $(AUX_CFLAGS) hdf-browse.c

globalhist.o: globalhist.c
	$(CC) -c $(CFLAGS) $(AUX_CFLAGS) globalhist.c

Mosaic: $(OBJS) $(AUXOBJS) $(HDFOBJS) $(PROGRAM_LIBS) $(DTM_LIBS)
	$(PURIFY) $(CC) -o Mosaic $(OBJS) $(AUXOBJS) $(HDFOBJS) $(LIBS)

HFILES = mosaic.h
$(OBJS): $(HFILES)
gui.o: xresources.h

wipe:
	-rm -f Mosaic $(OBJS) core
clean:
	-rm -f Mosaic *.o core
tags:
	etags -t *.[ch]
