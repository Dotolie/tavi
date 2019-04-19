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
CFLAGS = -Wall $(WARNINGS) -O2 -D__KERNEL__ -D__linux__ -DLINUX -DEXPORT_SYMTAB -DMODULE -DLINUX -I$(UCLINUXDIR) -fno-builtin $(CFLAGS_OPT)
LDFLAGS = -r

USER_CFLAGS = -O2 -I$(UCLINUXDIR)
USER_LDFLAGS = -Wl,-elf2flt="-s32768"

all: misc_ctrl.o

misc_ctrl.o: misc_ctrl.c misc_ctrl.h
	$(CC) $(CFLAGS) -c -o $@ misc_ctrl.c

misctest: misc_test.cpp
	$(CC) $(USER_CFLAGS) $(USER_LDFLAGS) -o $@ misc_test.cpp 
	cp misctest $(EM85xx_ROOT)/romfs

install: misc_ctrl.o
	mkdir -p $(DRIVERDIR)
	cp misc_ctrl.o $(DRIVERDIR)

clean:
	rm -f *.bin *.gdb *~ *.o

