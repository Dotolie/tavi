##********************** Copyright(C) New Media Life Inc. *********************
##
## Description : Extras Task Makefile
## 
## $Source: /home/cvsroot/TAVI020/tavi/Task/Extras/Makefile,v $
## $Revision: 1.2 $
## $Date: 2006/02/27 02:40:17 $
## $Author: akaiaphanu $
##
##*****************************************************************************
## $Log: Makefile,v $
## Revision 1.2  2006/02/27 02:40:17  akaiaphanu
## working
##
## Revision 1.7  2006/02/20 01:33:56  akaiaphanu
## add ListManager , BitIO
##
## Revision 1.6  2006/02/11 07:19:11  akaiaphanu
## work for dictionary
##
## Revision 1.5  2005/11/21 02:48:29  etnlwind
## Update Makefile
##
## Revision 1.4  2005/11/07 06:30:21  yong
## add CAlarmSet, FileSelect
##
## Revision 1.3  2005/10/12 01:57:38  akaiaphanu
## Add to log Information
##
##*****************************************************************************

TAVI_DIR = $(EM85xx_ROOT)/tavi
TAVI_INCLUDE = $(TAVI_DIR)/include

#export OBJ_DIR CC CFLAGS LDFLAGS PVP_DIR TAVI_DIR TAVI_INCLUDE

all : $(OBJ_DIR)/Extra.o \
	$(OBJ_DIR)/CExtraClock.o \
	$(OBJ_DIR)/AlarmSet.o \
	$(OBJ_DIR)/CExtraDic.o \
	$(OBJ_DIR)/DicKeyboard.o \
	$(OBJ_DIR)/vkbd.o \
	$(OBJ_DIR)/HistoryManager.o \
	$(OBJ_DIR)/DataManager.o \
	$(OBJ_DIR)/ListManager.o \
	$(OBJ_DIR)/BitIO.o

$(OBJ_DIR)/Extra.o : Extra.h Extra.cpp
	$(CC) $(CFLAGS) -o $@ -c Extra.cpp  -I$(TAVI_DIR) -I$(TAVI_INCLUDE)
$(OBJ_DIR)/CExtraClock.o : CExtraClock.h CExtraClock.cpp
	$(CC) $(CFLAGS) -o $@ -c CExtraClock.cpp  -I$(TAVI_DIR) -I$(TAVI_INCLUDE)
$(OBJ_DIR)/CExtraDic.o : CExtraDic.h CExtraDic.cpp
	$(CC) $(CFLAGS) -o $@ -c CExtraDic.cpp  -I$(TAVI_DIR) -I$(TAVI_INCLUDE)
$(OBJ_DIR)/DicKeyboard.o : DicKeyboard.h DicKeyboard.cpp
	$(CC) $(CFLAGS) -o $@ -c DicKeyboard.cpp  -I$(TAVI_DIR) -I$(TAVI_INCLUDE)
$(OBJ_DIR)/vkbd.o : vkbd.h vkbd.cpp
	$(CC) $(CFLAGS) -o $@ -c vkbd.cpp  -I$(TAVI_DIR) -I$(TAVI_INCLUDE)
$(OBJ_DIR)/HistoryManager.o : HistoryManager.h HistoryManager.cpp
	$(CC) $(CFLAGS) -o $@ -c HistoryManager.cpp  -I$(TAVI_DIR) -I$(TAVI_INCLUDE)
$(OBJ_DIR)/DataManager.o : DataManager.h DataManager.cpp
	$(CC) $(CFLAGS) -o $@ -c DataManager.cpp  -I$(TAVI_DIR) -I$(TAVI_INCLUDE)
$(OBJ_DIR)/ListManager.o : ListManager.h ListManager.cpp
	$(CC) $(CFLAGS) -o $@ -c ListManager.cpp  -I$(TAVI_DIR) -I$(TAVI_INCLUDE)
$(OBJ_DIR)/BitIO.o : BitIO.h BitIO.cpp
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ -c BitIO.cpp  -I$(TAVI_DIR) -I$(TAVI_INCLUDE)
$(OBJ_DIR)/AlarmSet.o : AlarmSet.h AlarmSet.cpp
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ -c AlarmSet.cpp  -I$(TAVI_DIR) -I$(TAVI_INCLUDE)

clean:
	rm -f *.o

# END
