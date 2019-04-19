# TAVI-PMP Version 1.0
# new media LIF CO., LTD.
# Makefile for TAVI
# Sang-jin Ryu (ew@newmedialife.com)
#
# $Log: Makefile,v $
# Revision 1.1.1.1  2006/02/22 12:33:31  zstein
# TAVI020 REBUILD
#
# Revision 1.7  2006/02/18 01:46:09  ywkim
# Remove subdir of midi
#
# Revision 1.6  2006/02/16 05:08:59  ywkim
# Add cleanning routine for subdir
#
# Revision 1.5  2006/02/07 13:08:29  ywkim
# export with CFLAGS_OPT
#
# Revision 1.4  2005/08/25 06:09:30  ywkim
# Merge to main trunk
#
# Revision 1.2.2.7  2005/08/25 04:36:39  ywkim
# Add subdir of midi
#
# Revision 1.2.2.6  2005/08/17 10:39:57  ywkim
# remove subdir midi
#
# Revision 1.2.2.5  2005/08/17 08:16:49  etnlwind
# Update for Sound Palette
#
# Revision 1.2.2.4  2005/04/19 11:26:42  zstein
# deleted sublcd.
#
# Revision 1.2.2.3  2005/04/11 07:19:02  zstein
# Not changed.
#
# Revision 1.2.2.2  2005/03/29 03:29:08  ywkim
# Add subdir for rtc & sublcd
#
# Revision 1.2.2.1  2005/03/03 12:29:31  zstein
# Added Misc driver
#

ARCH = arm-elf-

CC     = $(ARCH)gcc
AR     = $(ARCH)ar
OBJCOPY= $(ARCH)objcopy
CXX    = $(ARCH)g++
LD     = $(ARCH)gcc
SHELL  = bash
RM     = rm -f

#GCC stuff
GCC_LIB = $(shell $(CC) --print-libgcc-file-name)
GCC_INC = $(dir $(GCC_LIB))include

#SUB_DIR = key touchpad sound i2cm fmradio misc rtc midi
SUB_DIR = key touchpad sound i2cm fmradio misc rtc

.PHONY : $(SUB_DIR) all clear

export OBJ_DIR CC CFLAGS LDFLAGS PVP_DIR TAVI_DIR TAVI_INCLUDE CFLAGS_OPT
#export OBJ_DIR CC CFLAGS LDFLAGS

all : $(SUB_DIR) 

$(SUB_DIR):
	cd $@ ; $(MAKE) install


clean :
	$(RM) $(OBJ_DIR)/*.o
	@for dir in $(SUB_DIR);\
	do $(MAKE) -C $$dir clean; \
	done

