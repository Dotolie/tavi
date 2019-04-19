##********************** Copyright(C) New Media Life Inc. *********************
##
## Description : Setup General Window class
## 
## $Source: /home/cvsroot/TAVI020/tavi/Task/setup/Makefile,v $
## $Revision: 1.1.1.1 $
## $Date: 2006/02/22 12:33:31 $
## $Author: zstein $
##
##*****************************************************************************
## $Log: Makefile,v $
## Revision 1.1.1.1  2006/02/22 12:33:31  zstein
## TAVI020 REBUILD
##
## Revision 1.12  2005/12/15 08:27:13  akaiaphanu
## Updated to brightness & contrast functions
##
## Revision 1.11  2005/11/24 09:12:59  ywkim
## Change some items of setup related sound EQ
## Add new classes of CSetupUserEQ & CSetupUserEffect
##
## Revision 1.10  2005/08/03 05:30:05  ywkim
## Add new item of TVadjust for output setup
##
## Revision 1.9  2005/07/18 06:48:16  ywkim
## Add item of Output & change other items
##
## Revision 1.8  2005/04/29 11:17:35  ywkim
## Collecting CVS parameter
##
##*****************************************************************************

TAVI_DIR = $(EM85xx_ROOT)/tavi
TAVI_INCLUDE = $(TAVI_DIR)/include

#export OBJ_DIR CC CFLAGS LDFLAGS PVP_DIR TAVI_DIR TAVI_INCLUD

all : $(OBJ_DIR)/setup.o \
	$(OBJ_DIR)/CTvAdjust.o \
	$(OBJ_DIR)/CSetupWnd.o \
	$(OBJ_DIR)/CSetupGeneral.o\
	$(OBJ_DIR)/CSetupDisplay.o\
	$(OBJ_DIR)/CSetupOutput.o\
	$(OBJ_DIR)/CSetupSound.o\
	$(OBJ_DIR)/CSetupPower.o\
	$(OBJ_DIR)/CSetupClock.o\
	$(OBJ_DIR)/CSetupUserEffect.o\
	$(OBJ_DIR)/CSetupUserEQ.o\
	$(OBJ_DIR)/CSetupAdjustBrightWnd.o


$(OBJ_DIR)/setup.o : setup.h setup.cpp
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ -c setup.cpp  -I$(TAVI_DIR) -I$(TAVI_INCLUDE)

$(OBJ_DIR)/CTvAdjust.o : CTvAdjust.h CTvAdjust.cpp
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ -c CTvAdjust.cpp  -I$(TAVI_DIR) -I$(TAVI_INCLUDE)

$(OBJ_DIR)/CSetupWnd.o : CSetupWnd.h CSetupWnd.cpp
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ -c CSetupWnd.cpp  -I$(TAVI_DIR) -I$(TAVI_INCLUDE)

$(OBJ_DIR)/CSetupGeneral.o : CSetupGeneral.h CSetupGeneral.cpp
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ -c CSetupGeneral.cpp  -I$(TAVI_DIR) -I$(TAVI_INCLUDE)

$(OBJ_DIR)/CSetupDisplay.o : CSetupDisplay.h CSetupDisplay.cpp
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ -c CSetupDisplay.cpp  -I$(TAVI_DIR) -I$(TAVI_INCLUDE)

$(OBJ_DIR)/CSetupOutput.o : CSetupOutput.h CSetupOutput.cpp
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ -c CSetupOutput.cpp  -I$(TAVI_DIR) -I$(TAVI_INCLUDE)

$(OBJ_DIR)/CSetupSound.o : CSetupSound.h CSetupSound.cpp
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ -c CSetupSound.cpp  -I$(TAVI_DIR) -I$(TAVI_INCLUDE)

$(OBJ_DIR)/CSetupPower.o : CSetupPower.h CSetupPower.cpp
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ -c CSetupPower.cpp  -I$(TAVI_DIR) -I$(TAVI_INCLUDE)

$(OBJ_DIR)/CSetupClock.o : CSetupClock.h CSetupClock.cpp
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ -c CSetupClock.cpp  -I$(TAVI_DIR) -I$(TAVI_INCLUDE)

$(OBJ_DIR)/CSetupUserEffect.o : CSetupUserEffect.h CSetupUserEffect.cpp
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ -c CSetupUserEffect.cpp  -I$(TAVI_DIR) -I$(TAVI_INCLUDE)

$(OBJ_DIR)/CSetupUserEQ.o : CSetupUserEQ.h CSetupUserEQ.cpp
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ -c CSetupUserEQ.cpp  -I$(TAVI_DIR) -I$(TAVI_INCLUDE)

$(OBJ_DIR)/CSetupAdjustBrightWnd.o : CSetupAdjustBrightWnd.h CSetupAdjustBrightWnd.cpp
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ -c CSetupAdjustBrightWnd.cpp  -I$(TAVI_DIR) -I$(TAVI_INCLUDE)