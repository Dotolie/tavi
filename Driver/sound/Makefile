# Makefile for TAVI Key driver Kernel Module
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

KERNELMODULEDIR = $(EM85xx_ROOT)/kernelmodule/realmagichwl_kernelland
KERNELUSERDIR = $(EM85xx_ROOT)/kernelmodule/realmagichwl_userland
UCLINUXDIR = $(EM85xx_ROOT)/uClinux-2.4/include

RUALIB = $(KERNELUSERDIR)/realmagichwl_userland_api.o

CFLAGS = -Wall $(WARNINGS) -O2 -D__KERNEL__ -D__linux__ -DMODULE -DLINUX -DEXPORT_SYMTAB -I$(UCLINUXDIR) -fno-builtin 
LDFLAGS = -r

USER_CFLAGS = -O2 -I$(UCLINUXDIR) -I$(KERNELMODULEDIR) -I$(KERNELUSERDIR)
USER_LDFLAGS = -Wl,-elf2flt="-s32768"

LIBS = $(RUALIB)
OBJS = sndtest.o 

all: sound.o

sound.o: sound.c sound.h
	$(CC) $(CFLAGS) -c -o $@ sound.c

sndtest: $(OBJS)
	$(CC) $(USER_CFLAGS) $(USER_LDFLAGS) -o $@ $(OBJS) $(LIBS)

sndtest.o: sndtest.c
	$(CC) $(USER_CFLAGS) -c $<

em85xx.o: mpegdec/em85xx.cpp
	$(CC) $(USER_CFLAGS) -c $<

install: sndtest sound.o
	mkdir -p $(DRIVERDIR)
	cp sound.o $(DRIVERDIR)

clean:
	rm -f *.bin *.gdb *~ *.o
