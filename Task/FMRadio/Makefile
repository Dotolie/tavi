##********************** Copyright(C) New Media Life Inc. *********************
##
## Description : Makefile of FMRadio
## 
## $Source: /home/cvsroot/TAVI020/tavi/Task/FMRadio/Makefile,v $
## $Revision: 1.1.1.1 $
## $Date: 2006/02/22 12:33:31 $
## $Author: zstein $
##
##*****************************************************************************
## $Log: Makefile,v $
## Revision 1.1.1.1  2006/02/22 12:33:31  zstein
## TAVI020 REBUILD
##
## Revision 1.2  2005/04/29 11:19:57  ywkim
## Insert CVS header
##
##*****************************************************************************

TAVI_DIR = $(EM85xx_ROOT)/tavi
TAVI_INCLUDE = $(TAVI_DIR)/include

export OBJ_DIR CC CFLAGS LDFLAGS TAVI_DIR TAVI_INCLUDE


all : $(OBJ_DIR)/FMRadio.o FMRadio.cpp FMRadio.h 

$(OBJ_DIR)/FMRadio.o : FMRadio.cpp FMRadio.h
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ -c FMRadio.cpp  -I$(TAVI_DIR) -I$(TAVI_INCLUDE)

clean:
	rm -rf *.o *.bin *.gdb *~ *.a
	rm -rf $(TAVI_OBJS)/*.o *.bin *.gdb 
