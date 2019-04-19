# Makefile for TAVI Touchpad(ScrollStrip) driver Kernel Module
# Copyright(c) 2004 New Media Life, Inc.
# Lee Seok<zstein@kiscos.net>

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


all: tavi_tp.o 

tavi_tp.o: tavi_tp.c 
	$(CC) $(CFLAGS) -c -o $@ tavi_tp.c

meplib.o: meplib.c meplib.h
	$(CC) $(USER_CFLAGS) -c meplib.c

tptest: tavi_tptest.c tavi_tp.h
	$(CC) $(USER_CFLAGS) $(USER_LDFLAGS) -o $@ tptest.c
	

install: tavi_tp.o meplib.o
	mkdir -p $(DRIVERDIR)
	cp tavi_tp.o $(DRIVERDIR)
	cp meplib.o $(OBJ_DIR)
#	cp meplib.h tavi_tp.h $(TAVI_INCLUDE)

clean:
	rm -f *.bin *.gdb *~ *.o
