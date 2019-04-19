##************************ Copyright(C) New Media Life Inc. *******************
##
## Description : Makefile for FM Radio(GNT-702S) Driver 
## 
## $Source: /home/cvsroot/TAVI020/tavi/Driver/fmradio/Makefile,v $
## $Revision: 1.1.1.1 $
## $Date: 2006/02/22 12:33:31 $
## $Author: zstein $
##
##*****************************************************************************
## $Log: Makefile,v $
## Revision 1.1.1.1  2006/02/22 12:33:31  zstein
## TAVI020 REBUILD
##
## Revision 1.4  2005/09/28 03:34:31  zstein
## - changed copy-path.
##
## Revision 1.3  2005/02/11 02:41:03  ywkim
## modify header
##
## Revision 1.2  2005/02/11 01:18:18  ywkim
## Modify parts of header
##
##*****************************************************************************

CC = arm-elf-gcc
LD = arm-elf-ld
AR = arm-elf-ar
WARNINGS = \
 -Wpointer-arith \
 -Wchar-subscripts \
 -Wsign-compare \
 -fcheck-new 

UCLINUXDIR = $(EM85xx_ROOT)/uClinux-2.4/include
CFLAGS = -Wall $(WARNINGS) -O2 -D__KERNEL__ -D__linux__ -DMODULE -DLINUX -DEXPORT_SYMTAB -DMODVERSIONS -I$(UCLINUXDIR) -fno-builtin 
LDFLAGS = -r

USER_CFLAGS = -O2 -I$(UCLINUXDIR)
USER_LDFLAGS = -Wl,-elf2flt="-s32768"

all: tavi_fm.o fmtest

tavi_fm.o: tavi_fm.h tavi_fm.c 
	$(CC) $(CFLAGS) -c -o $@ tavi_fm.c

fmtest.o: fmtest.h fmtest.c
	$(CC) $(USER_CFLAGS) -c -o $@ fmtest.c

fmtest: fmtest.o
	$(CC) $(USER_LDFLAGS) -o $@ $<

install: tavi_fm.o
	mkdir -p $(DRIVERDIR)
	cp tavi_fm.o $(DRIVERDIR)

test: tavi_fm.o fmtest
	cp tavi_fm.o $(EM85xx_ROOT)/romfs
	cp fmtest $(EM85xx_ROOT)/romfs
	$(MAKE) -C $(EM85xx_ROOT) genromfs

clean:
	rm -f *.bin *.gdb *~ *.o
