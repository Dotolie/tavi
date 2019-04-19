##********************** Copyright(C) New Media Life Inc. *********************
##
## Description : Emersys 3D sound 
## 
## $Source: /home/cvsroot/TAVI020/tavi/3Dsound/Makefile,v $
## $Revision: 1.1.1.1 $
## $Date: 2006/02/22 12:33:31 $
## $Author: zstein $
##
##*****************************************************************************
## $Log: Makefile,v $
## Revision 1.1.1.1  2006/02/22 12:33:31  zstein
## TAVI020 REBUILD
##
## Revision 1.2  2005/10/19 02:07:35  ywkim
## Change .cpp to .c for sharing to kernelmodule
##
## Revision 1.1  2005/06/17 08:18:31  ywkim
## initially adding for emersys 3d
##
##*****************************************************************************

TAVI_DIR = $(EM85xx_ROOT)/tavi
TAVI_INCLUDE = $(TAVI_DIR)/include


all : $(OBJ_DIR)/3Dsound.o


$(OBJ_DIR)/3Dsound.o : 3Dsound.h 3Dsound.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ -c 3Dsound.c -I$(TAVI_DIR) -I$(TAVI_INCLUDE)
