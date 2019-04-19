# Makefile for TAVI Key driver Kernel Module
# Copyright(c) 2004 New Media Life, Inc.
# Lee Seok<zstein@kiscos.net>
# $Log: Makefile,v $
# Revision 1.1.1.1  2006/02/22 12:33:31  zstein
# TAVI020 REBUILD
#
# Revision 1.9  2005/09/28 03:35:13  zstein
# - changed copy-path.
#
# Revision 1.8  2005/08/29 07:44:03  zstein
# - removed test program
#
# Revision 1.7  2005/08/25 05:52:59  ywkim
# merge & correct source
#
# Revision 1.6  2005/03/05 12:00:54  etnlwind
# key version check
#
# Revision 1.5  2005/03/05 08:00:44  zstein
# *** empty log message ***
#

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

all: tavi_key.o

tavi_key.o: tavi_key.c tavi_key.h
	$(CC) $(CFLAGS) -c -o $@ tavi_key.c

keytest: tavi_keytest.c tavi_key.h
	$(CC) $(USER_CFLAGS) $(USER_LDFLAGS) -o $@ tavi_keytest.c
	

install: tavi_key.o
	mkdir -p $(DRIVERDIR)
	cp tavi_key.o $(DRIVERDIR)
#	cp keytest $(EM85xx_ROOT)/romfs 
#	cp tavi_key.h $(TAVI_INCLUDE)

clean:
	rm -f *.bin *.gdb *~ *.o


