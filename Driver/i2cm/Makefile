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

UCLINUXDIR = $(EM85xx_ROOT)/uClinux-2.4/include
CFLAGS = -Wall $(WARNINGS) -O2 -D__KERNEL__ -D__linux__ -DMODULE -DLINUX -DEXPORT_SYMTAB -DMODVERSIONS -I$(UCLINUXDIR) -fno-builtin 
LDFLAGS = -r

USER_CFLAGS = -O2 -I$(UCLINUXDIR)
USER_LDFLAGS = -Wl,-elf2flt="-s32768"

all: i2cm.o 

i2cm.o: i2cm.c i2cm.h
	$(CC) $(CFLAGS) -c -o $@ i2cm.c

install: i2cm.o
	mkdir -p $(DRIVERDIR)
	cp i2cm.o $(DRIVERDIR)

test: i2cm.o 
	cp i2cm.o $(EM85xx_ROOT)/romfs
	$(MAKE) -C $(EM85xx_ROOT) genromfs

clean:
	rm -f *.bin *.gdb *~ *.o
