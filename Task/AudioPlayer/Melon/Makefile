##********************** Copyright(C) New Media Life Inc. *********************
##
## Description : Melon basic api set for tavi020 
## 
## $Source: /home/cvsroot/TAVI020/tavi/Task/AudioPlayer/Melon/Makefile,v $
## $Revision: 1.1 $
## $Date: 2006/06/12 04:33:50 $
## $Author: ywkim $
##
##*****************************************************************************
## $Log: Makefile,v $
## Revision 1.1  2006/06/12 04:33:50  ywkim
## Initial release for melon
##
##*****************************************************************************

TAVI_DIR = $(EM85xx_ROOT)/tavi
TAVI_INCLUDE = $(TAVI_DIR)/include
MELONDIR = $(EM85xx_ROOT)/libraries/libmelon
MELONLIB = $(MELONDIR)/libmelon.a

MELONOBJ = MelonIO.o Melon.o

all : $(MELONOBJ)


MelonIO.o : MelonIO.cpp MelonIO.h
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ -c MelonIO.cpp -I$(TAVI_DIR) -I$(TAVI_INCLUDE) -I$(MELONDIR)

Melon.o : Melon.cpp Melon.h
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ -c Melon.cpp -I$(TAVI_DIR) -I$(TAVI_INCLUDE) -I$(MELONDIR)

clean:
	rm -f *.o *.gdb
