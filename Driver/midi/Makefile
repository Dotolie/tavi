##************************ Copyright(C) New Media Life Inc. *******************
##
## Description : Makefile for Hwa sound source Driver 
## 
## $Source: /home/cvsroot/TAVI020/tavi/Driver/midi/Makefile,v $
## $Revision: 1.1.1.1 $
## $Date: 2006/02/22 12:33:31 $
## $Author: zstein $
##
##*****************************************************************************
## $Log: Makefile,v $
## Revision 1.1.1.1  2006/02/22 12:33:31  zstein
## TAVI020 REBUILD
##
## Revision 1.3  2005/09/28 03:35:27  zstein
## - changed copy-path.
##
## Revision 1.2  2005/08/25 04:54:43  ywkim
## initial release
##
## Revision 1.1.2.1  2005/08/25 04:38:03  ywkim
## initial release for reducing power consumtion
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
CFLAGS = -Wall $(WARNINGS) -O2 -D__KERNEL__ -D__linux__ -DMODULE -DLINUX -I$(UCLINUXDIR) -fno-builtin 
LDFLAGS = -r

USER_CFLAGS = -O2 -I$(UCLINUXDIR)
USER_LDFLAGS = -Wl,-elf2flt="-s32768"

all: tavi_midi.o 

tavi_midi.o: tavi_midi.c tavi_midi.h
	$(CC) $(CFLAGS) -c -o $@ tavi_midi.c

miditest.bin: miditest.c miditest.h
	$(CC) $(USER_CFLAGS) $(USER_LDFLAGS) -o $@ miditest.c
	

install: tavi_midi.o
	mkdir -p $(DRIVERDIR)
	cp tavi_midi.o $(DRIVERDIR)
#	cp miditest.bin $(EM85xx_ROOT)/romfs 
#	cp tavi_key.h $(TAVI_INCLUDE)

clean:
	rm -f *.bin *.gdb *~ *.o
