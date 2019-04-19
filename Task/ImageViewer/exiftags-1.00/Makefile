# $Id: Makefile,v 1.1.1.1 2006/02/22 12:33:31 zstein Exp $

#
# Add any new maker note modules here.
#
MKRS=makers.o canon.o olympus.o fuji.o nikon.o casio.o minolta.o \
	sanyo.o asahi.o leica.o panasonic.o sigma.o

#
# Or, if you don't want maker note modules...
#
NOMKRS=makers_stub.o

#
# A few parameters...
#
OBJS=exif.o tagdefs.o exifutil.o exifgps.o jpeg.o
HDRS=exif.h exifint.h jpeg.h makers.h


.SUFFIXES: .o .c
.c.o:
	$(CC) $(CFLAGS) -o $@ -c $<


all: $(OBJ_DIR)/libexiftag.a

$(OBJ_DIR)/libexiftag.a: $(OBJS) $(MKRS) $(HDRS)
	$(AR) -rv $(OBJ_DIR)/libexiftag.a $(OBJS) $(MKRS)

