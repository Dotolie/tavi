##************************ Copyright(C) New Media Life Inc. *******************
##
## Description : Makefile for RTC(PCF-8563TS) Driver 
## 
## $Source: /home/cvsroot/TAVI020/tavi/Driver/rtc/Makefile,v $
## $Revision: 1.1.1.1 $
## $Date: 2006/02/22 12:33:31 $
## $Author: zstein $
##
##*****************************************************************************
## $Log: Makefile,v $
## Revision 1.1.1.1  2006/02/22 12:33:31  zstein
## TAVI020 REBUILD
##
## Revision 1.4  2005/09/28 03:35:45  zstein
## - changed copy-path.
##
## Revision 1.3  2005/08/18 12:19:49  zstein
## *** empty log message ***
##
## Revision 1.2  2005/08/12 03:12:53  zstein
## - T0
##
## Revision 1.1  2005/03/30 06:13:10  ywkim
## Initially add
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

all: tavi_rtc.o

tavi_rtctest.bin: tavi_rtctest.c tavi_rtc.h
	$(CC) $(USER_CFLAGS) $(USER_LDFLAGS) -o tavi_rtctest.bin tavi_rtctest.c

tavi_rtc.o: tavi_rtc.c tavi_rtc.h
	$(CC) $(CFLAGS) -c -o $@ tavi_rtc.c

test: tavi_rtc.o tavi_rtctest.bin
	cp tavi_rtc.o $(EM85xx_ROOT)/romfs
	cp tavi_rtctest.bin $(EM85xx_ROOT)/romfs/bin/rtctest
	$(MAKE) -C $(EM85xx_ROOT) genromfs

install: tavi_rtc.o 
	mkdir -p $(DRIVERDIR)
	cp tavi_rtc.o $(DRIVERDIR)

clean:
	rm -f *.bin *.gdb *~ *.o
