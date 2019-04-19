#
# makefile for sample TAVI application
# $Id: Makefile,v 1.8 2006/06/07 08:32:20 zstein Exp $
# $Author: zstein $
# $Date: 2006/06/07 08:32:20 $
# $Log: Makefile,v $
# Revision 1.8  2006/06/07 08:32:20  zstein
# *** empty log message ***
#
# Revision 1.7  2006/04/15 03:11:33  zstein
# - modified clean rule.
#
# Revision 1.6  2006/04/05 01:58:06  zstein
# - added option MEM64MB
#
# Revision 1.5  2006/03/21 01:46:01  zstein
# - mfileio.cpp, bytestream.cpp
#
# Revision 1.4  2006/03/06 10:42:01  zstein
# *** empty log message ***
#
# Revision 1.3  2006/02/23 06:58:25  ywkim
# Remove ..\VERSION_INFO for generation of version.h
#
# Revision 1.2  2006/02/23 01:13:40  zstein
# - changed version.h rule.
#
# Revision 1.1  2006/02/22 13:04:58  zstein
# *** empty log message ***
#
# Revision 1.1.1.1  2006/02/22 12:33:31  zstein
# TAVI020 REBUILD
#
# Revision 1.39  2006/02/17 06:49:29  ywkim
# Change type of PRODUCTID to char.
#
# Revision 1.38  2006/02/16 05:18:16  ywkim
# Add cleanning routine for sub dir of Driver
#
# Revision 1.37  2006/02/16 03:14:43  ywkim
# Change macro name of TAVI1600_xxx to TAVI020_xxx
#
# Revision 1.36  2006/02/10 10:56:23  ywkim
# Add macro of CFLAGS_OPT += -DCHARGER
#
# Revision 1.35  2006/02/08 01:00:03  ywkim
# Remark CFLAGS_OPT for old pcb
#
# Revision 1.34  2006/02/07 13:09:46  ywkim
# Add CFLAGS_OPT for HW option
#
# Revision 1.33  2006/02/04 06:01:41  ywkim
# Add macro for definition of HW0602
#
# Revision 1.32  2006/01/31 12:59:05  zstein
# *** empty log message ***
#
# Revision 1.31  2006/01/16 02:28:46  ywkim
# Add MACRO of CFLAGS for HW0601
#
# Revision 1.30  2005/11/22 05:41:30  ywkim
# Add subdir md5 for MD5 key
#
# Revision 1.29  2005/11/21 02:51:16  etnlwind
# Update for EQ Select UI
#
# Revision 1.28  2005/11/16 08:48:46  etnlwind
# Update for AlarmSet
#
# Revision 1.27  2005/09/28 03:33:30  zstein
# - update
#
# Revision 1.26  2005/09/09 05:47:59  zstein
# - update
#
# Revision 1.25  2005/08/22 12:16:09  zstein
# - added clean
#
# Revision 1.24  2005/07/20 06:10:47  zstein
# - added libmath
#
# Revision 1.23  2005/07/11 00:12:26  zstein
# - added mp3encode library.
#
# Revision 1.22  2005/06/21 05:12:38  ywkim
# change XENLIB path
#
# Revision 1.21  2005/06/20 12:25:09  ywkim
# Add XENLIB,XENDIR path & routine for 3d sound
#
# Revision 1.20  2005/05/27 09:05:42  zstein
# - deleted ogg/wma
#
# Revision 1.19  2005/05/13 05:51:33  ywkim
# Remove definition of version
#
# Revision 1.18  2005/05/07 02:13:44  zstein
# delete libgdbm.a
#
# Revision 1.17  2005/04/28 12:33:48  etnlwind
# Update for ES4
#
# Revision 1.16  2005/04/23 06:14:24  zstein
# changed make order.
#
# Revision 1.15  2005/04/18 12:55:53  etnlwind
# update for ES4
#
# Revision 1.14  2005/04/14 03:09:09  zstein
# *** empty log message ***
#
# Revision 1.13  2005/04/13 10:31:53  zstein
# Added fatfs.o in $OBJS
#
# Revision 1.12  2005/03/31 08:13:59  ywkim
# Remove EMX lib path
#
# Revision 1.11  2005/03/21 06:56:10  ywkim
# Remove settings.cpp
#
# Revision 1.10  2005/03/15 04:00:46  ywkim
# modify loadmodule & patchlevel for cebit 0.1.2.0
#
# Revision 1.9  2005/03/10 04:04:11  zstein
# *** empty log message ***
#
# Revision 1.8  2005/03/06 19:23:23  ywkim
# modify minor version no to 1 for cevit
#
# Revision 1.7  2005/03/05 12:52:49  ywkim
# Add version.h for managing version
#
# Revision 1.6  2005/03/03 04:58:13  etnlwind
# VideoPlayer, FileBrowser 부분 수정
#
# Revision 1.5  2005/03/02 12:25:52  zstein
# Remove dac
#
#
//MAJORVERSION = 0
//MINORVERSION = 2
//PATCHLEVEL1 = 0
//PATCHLEVEL2 = 0
SERIALNO = T20NM5C00171G

TAVI_DIR  = $(EM85xx_ROOT)/tavi
TAVI_OBJS = $(TAVI_DIR)/objs
TAVI_INCLUDE = $(TAVI_DIR)/include


UCLINUX_LDFLAGS = -Wl,-elf2flt="-s32768"
UCLINUXDIR = $(EM85xx_ROOT)/uClinux-2.4/include
DRIVERDIR = $(EM85xx_ROOT)/romfs/lib/modules

KERNELMODULEDIR = $(EM85xx_ROOT)/kernelmodule/realmagichwl_kernelland
KERNELUSERDIR = $(EM85xx_ROOT)/kernelmodule/realmagichwl_userland
DPIDIR = $(EM85xx_ROOT)/dpi
JPEGDIR = $(EM85xx_ROOT)/thirdparties/libjpeg
JPEGLIB = $(JPEGDIR)/libjpeg.a
MP3DIR  = $(EM85xx_ROOT)/libraries/libmad
COMPONENTDIR = $(EM85xx_ROOT)/tavi/Component
MP3LIB  = $(MP3DIR)/libmad.a
EQUDIR  = $(EM85xx_ROOT)/libraries/libEqualizer
EQULIB  = $(EQUDIR)/equalizer.a
#WMADIR  = $(EM85xx_ROOT)/libraries/libwma
#WMALIB  = $(WMADIR)/libwma.a
OGGDIR  = $(EM85xx_ROOT)/thirdparties/libtremor
OGGLIB  = $(OGGDIR)/libtremor.a
MP43DIR = $(EM85xx_ROOT)/libraries/libmp43
MP43LIB = $(MP43DIR)/core.a
AACDIR = $(EM85xx_ROOT)/libraries/libaac
AACLIB = $(AACDIR)/libaac.a
MPGDEMUXDIR = $(EM85xx_ROOT)/libraries/libmpgdmx
MPGDEMUXLIB = $(EM85xx_ROOT)/libraries/libmpgdmx/mpgdemux.a
FATFSDIR = $(EM85xx_ROOT)/libraries/libfatfs
FATFSLIB = $(FATFSDIR)/fatfs.a
ATADRVDIR = $(EM85xx_ROOT)/atadrv
ATAPIDRVDIR = $(EM85xx_ROOT)/atapi
PBIDRVDIR = $(EM85xx_ROOT)/pbi_ide
MP43SCANDIR = $(EM85xx_ROOT)/libraries/libmp43scan
MP43SCANLIB = $(MP43SCANDIR)/mp43scan.a
RUALIB	= $(KERNELUSERDIR)/realmagichwl_userland_api.o
XENDIR = $(TAVI_DIR)/3Dsound
XENLIB = $(XENDIR)/libXen.a
MP3ENCLIB = $(EM85xx_ROOT)/libraries/libmp3enc/libmp3enc.a
MD5DIR = $(TAVI_DIR)/md5
#LIBS    = $(JPEGLIB) $(MP3LIB) $(WMALIB) $(RUALIB) $(MP43LIB) $(EQULIB) $(OGGLIB) $(MPGDEMUXLIB) $(AACLIB) $(FATFSLIB) $(MP43SCANLIB)
LIBS    = $(JPEGLIB) $(MP3LIB) $(RUALIB) $(MP43LIB) $(MPGDEMUXLIB) $(AACLIB) $(FATFSLIB) $(MP43SCANLIB) $(XENLIB) $(MP3ENCLIB)


APP_ROOT = $(shell pwd)
OBJ_DIR = $(shell pwd)/objs

export DRIVERDIR

CC = arm-elf-gcc
CFLAGS = -O2 -fomit-frame-pointer -fexpensive-optimizations \
	-fthread-jumps -fmemoize-lookups -fschedule-insns2 \
	-finline-functions -ffunction-sections -fdata-sections
CFLAGS += -I$(UCLINUXDIR) -I$(JPEGDIR) -I$(MP3DIR) -I$(OGGDIR) -I$(MP43DIR) \
		  -I$(MPGDEMUXDIR) -I$(AACDIR) -I$(FATFSDIR) -I$(MP43SCANDIR) -I$(KERNELMODULEDIR)\
		  -I$(KERNELUSERDIR)  -I$(TAVI_DIR)/include -I$(EM85xx_ROOT)/tavi

CFLAGS_OPT += -DHW0601
CFLAGS_OPT += -DHW0602
CFLAGS_OPT += -DCHARGER
CFLAGS_OPT += -DMEM64MB

CFLAGS += $(CFLAGS_OPT)
CFLAGS += -DFATFS_PBI_V_3_0_3

LD = arm-elf-gcc
LDFLAGS = $(UCLINUX_LDFLAGS)
AR = arm-elf-ar

OBJS = main.o jpeg.o em85xx.o util.o mp3.o avi.o jasperlib.o lcd.o linuxfs.o queue.o mpgparse.o \
		ac3.o aac.o mp4.o bitstrm.o vol.o fs.o fs_fat.o md5.o mfileio.o bytestream.o


.PHONY : libmath Driver Component 3Dsound Task Lib 
export : CFLAGS  OBJ_DIR CC  LDFLAGS TAVI_DIR TAVI_INCLUDE CFLAGS_OPT


tavi.bin: version.h $(OBJS) jpeglib mp3lib mp43lib aaclib fatfslib libmath Component 3Dsound Lib Task Driver Md5
	$(LD) $(LDFLAGS) -s -r -nostdlib -o tavi.o $(OBJS) $(TAVI_OBJS)/*.o  $(TAVI_OBJS)/*.a $(LIBS) 
	arm-elf-strip -g -S -X -x tavi.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o tavi.bin tavi.o

version.h : ../Makefile
	@echo "/*" > $@
	@echo " * Tavi020 Program Version" >> $@
	@echo " *" >> $@
	@echo " * This file is automatically generated from makefile" >> $@
	@echo " */" >> $@
	@echo "" >> $@
	@echo "#ifndef __TAVI020_VERSION_H" >> $@
	@echo "#define __TAVI020_VERSION_H" >> $@
	@echo "" >> $@
	@echo "#define TAVI020_MAJORVERSION		$(MAJORVERSION)" >> $@
	@echo "#define TAVI020_MINORVERSION		$(MINORVERSION)" >> $@
	@echo "#define TAVI020_PATCHLEVEL1		$(PATCHLEVEL1)" >> $@
	@echo "#define TAVI020_PATCHLEVEL2		$(PATCHLEVEL2)" >> $@
	@echo "" >> $@
	@echo "#define MAKE_VERSION(a, b, c, d)	(((a) << 24) | ((b) << 16) | (c)<< 8 | (d) )" >> $@
	@echo "#define TAVI020_VERSION			MAKE_VERSION($(MAJORVERSION), $(MINORVERSION), $(PATCHLEVEL1), $(PATCHLEVEL2) )" >> $@
	@echo "#define TAVI020_VERSIONSTR		\"$(MAJORVERSION).$(MINORVERSION).$(PATCHLEVEL1).$(PATCHLEVEL2)\"" >> $@
	@echo "#define TAVI020_SERIALNO		\"$(SERIALNO)\"" >> $@
	@echo "#define TAVI020_PRODUCTID		\"$(PRODUCTID)\"" >> $@
	@echo "#define TAVI020_COMPILETIME	__DATE__\" \"__TIME__" >> $@
	@echo "" >> $@
	@echo "#endif" >> $@

test: tavi.bin
	cp tavi.bin ../romfs/bin/init/tavi.bin
	$(MAKE) -C ../ genromfs

libmath:
	$(MAKE) -C $(TAVI_DIR)/libmath
	
Driver:
	$(MAKE) -C $(TAVI_DIR)/Driver

Component :
	$(MAKE) -C $(TAVI_DIR)/Component

3Dsound :
	$(MAKE) -C $(TAVI_DIR)/3Dsound

Task:
	$(MAKE) -C $(TAVI_DIR)/Task
	
Lib:
	$(MAKE) -C  $(TAVI_DIR)/Lib
	
jpeglib:
	$(MAKE) -C $(JPEGDIR)

mp3lib:
	$(MAKE) -C $(MP3DIR)

wmalib:
	$(MAKE) -C $(WMADIR)

mp43lib:
	$(MAKE) -C $(MP43DIR)

ogglib:
	$(MAKE) -C $(OGGDIR)

aaclib:
	$(MAKE) -C $(AACDIR)

fatfslib:
	$(MAKE) -C $(FATFSDIR)

Md5:
	$(MAKE) -C $(MD5DIR)

main.o: main.cpp
	$(CC) $(CFLAGS) -c -o main.o main.cpp -I$(KERNELUSERDIR) -I$(KERNELMODULEDIR) -I$(WMADIR) -I$(WMADIR)/decoder -I$(MP43DIR) -I$(TAVI_DIR)

fs.o: fs.cpp
	$(CC) $(CFLAGS) -c -o fs.o fs.cpp

jpeg.o: jpeg/jpeg.cpp
	$(CC) $(CFLAGS) -c -o jpeg.o jpeg/jpeg.cpp

em85xx.o: mpegdec/em85xx.cpp
	$(CC) $(CFLAGS) -c -o em85xx.o mpegdec/em85xx.cpp -I$(KERNELMODULEDIR) -I$(KERNELUSERDIR)

util.o: util.cpp
	$(CC) $(CFLAGS) -c -o util.o util.cpp

mp3.o: mp3/mp3.cpp
	$(CC) $(CFLAGS) -c -o mp3.o mp3/mp3.cpp

wma.o: wma/wma.cpp
	$(CC) $(CFLAGS) -c -o wma.o wma/wma.cpp -I$(WMADIR) -I$(WMADIR)/decoder

avi.o: avi/avi.cpp
	$(CC) $(CFLAGS) -c -o avi.o avi/avi.cpp

#filebrowser.o: filebrowser/filebrowser.cpp
#	$(CC) $(CFLAGS) -c -o filebrowser.o filebrowser/filebrowser.cpp 

jasperlib.o: $(KERNELUSERDIR)/jasperlib.c
	$(CC) $(CFLAGS) -c -o jasperlib.o $(KERNELUSERDIR)/jasperlib.c

lcd.o: lcd/lcd.cpp
	$(CC) $(CFLAGS) -c -o lcd.o lcd/lcd.cpp

linuxfs.o: linuxfs.cpp
	$(CC) $(CFLAGS) -c -o linuxfs.o linuxfs.cpp

dpifs.o: dpifs.cpp
	$(CC) $(CFLAGS) -c -o dpifs.o dpifs.cpp -I$(DPIDIR)

queue.o: queue.cpp
	$(CC) $(CFLAGS) -c -o queue.o queue.cpp

mpgparse.o: mpgparse/mpgparse.cpp
	$(CC) $(CFLAGS) -c -o mpgparse.o mpgparse/mpgparse.cpp

ogg.o: ogg/ogg.cpp
	$(CC) $(CFLAGS) -c -o ogg.o ogg/ogg.cpp

ac3.o: ac3/ac3.cpp
	$(CC) $(CFLAGS) -c -o ac3.o ac3/ac3.cpp

aac.o: aac/aac.cpp
	$(CC) $(CFLAGS) -c -o aac.o aac/aac.cpp

mp4.o: mp4/mp4.cpp
	$(CC) $(CFLAGS) -c -o mp4.o mp4/mp4.cpp

bitstrm.o: bitstrm.cpp
	$(CC) $(CFLAGS) -c -o bitstrm.o bitstrm.cpp

vol.o: vol.cpp
	$(CC) $(CFLAGS) -c -o vol.o vol.cpp

md5.o: md5/md5.cpp md5/md5.h
	$(CC) $(CFLAGS) -c -o md5.o md5/md5.cpp

bytestream.o: bytestream.cpp bytestream.h
	$(CC) $(CFLAGS) -c -o bytestream.o bytestream.cpp

mfileio.o: mfileio.cpp mfileio.h
	$(CC) $(CFLAGS) -c -o mfileio.o mfileio.cpp


ifeq ($(ATA_DRV), 1)
fs_fat.o: fs_fat.cpp
	$(CC) $(CFLAGS) -c -o fs_fat.o fs_fat.cpp -I$(ATADRVDIR)
else
fs_fat.o: fs_fat.cpp
	$(CC) $(CFLAGS) -c -o fs_fat.o fs_fat.cpp -I$(PBIDRVDIR)
endif



clean:
	rm -rf *.o *.bin *.gdb *~ *.a
	rm -rf $(TAVI_OBJS)/*.o $(TAVI_OBJS)/*.a *.bin *.gdb 
	$(MAKE) -C Task clean
	$(MAKE) -C Driver clean
	$(MAKE) -C Component clean

