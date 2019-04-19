##********************** Copyright(C) New Media Life Inc. *********************
##
## Description : Genmd5key utility
## 
## $Source: /home/cvsroot/TAVI020/tavi/md5/Makefile,v $
## $Revision: 1.1.1.1 $
## $Date: 2006/02/22 12:33:31 $
## $Author: zstein $
##
##*****************************************************************************
## $Log: Makefile,v $
## Revision 1.1.1.1  2006/02/22 12:33:31  zstein
## TAVI020 REBUILD
##
## Revision 1.1  2005/11/22 05:54:01  ywkim
## Add new directory & source for MD5 key
##
##*****************************************************************************
CC = gcc
LD = ld
AR = ar


TAVI_DIR = $(EM85xx_ROOT)/tavi
TAVI_INCLUDE = $(TAVI_DIR)/include

all: genmd5key

genmd5key: genmd5key.o md5.o
	$(CC) -o $@ genmd5key.o md5.o

genmd5key.o: genmd5key.cpp 
	$(CC) -c -o $@ $<

md5.o: md5.cpp md5.h
	$(CC) -c -o $@ $<

clean:
	rm -f *.bin *.gdb *~ *.o
