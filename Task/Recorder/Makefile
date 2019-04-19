# Makefile for ImageViewer
# $Id: Makefile,v 1.1.1.1 2006/02/22 12:33:31 zstein Exp $
# $Source: /home/cvsroot/TAVI020/tavi/Task/Recorder/Makefile,v $
# $Revision: 1.1.1.1 $
# $Date: 2006/02/22 12:33:31 $
# $Author: zstein $ Lee Seok<zstein@newmedialife.com>
# $Log: Makefile,v $
# Revision 1.1.1.1  2006/02/22 12:33:31  zstein
# TAVI020 REBUILD
#
# Revision 1.4  2006/01/17 03:25:43  zstein
# *** empty log message ***
#
# Revision 1.3  2005/10/18 11:33:34  zstein
# - deleted RecTouchMenu.o
#
# Revision 1.2  2005/08/22 12:47:52  ywkim
# Change procedure for clean
#
# Revision 1.1  2005/07/16 03:09:37  zstein
# - start
#

TAVI_DIR = $(EM85xx_ROOT)/tavi
TAVI_INCLUDE = $(TAVI_DIR)/include

#export OBJ_DIR CC CFLAGS LDFLAGS PVP_DIR TAVI_DIR TAVI_INCLUD

all : $(OBJ_DIR)/RecorderMain.o $(OBJ_DIR)/RECMainWnd.o $(OBJ_DIR)/RecordWnd.o $(OBJ_DIR)/RECMainListView.o \
	$(OBJ_DIR)/RecordBottomWnd.o $(OBJ_DIR)/RecVolCtrl.o $(OBJ_DIR)/RecFolderView.o


$(OBJ_DIR)/RecorderMain.o : RecorderMain.h RecorderMain.cpp
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ -c RecorderMain.cpp  -I$(TAVI_DIR) -I$(TAVI_INCLUDE)

$(OBJ_DIR)/RECMainWnd.o : RECMainWnd.h RECMainWnd.cpp
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ -c RECMainWnd.cpp  -I$(TAVI_DIR) -I$(TAVI_INCLUDE)

$(OBJ_DIR)/RecordWnd.o : ../tavi_global.h RecordWnd.h RecordWnd.cpp
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ -c RecordWnd.cpp  -I$(TAVI_DIR) -I$(TAVI_INCLUDE)

$(OBJ_DIR)/RECMainListView.o : RECMainListView.h RECMainListView.cpp
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ -c RECMainListView.cpp  -I$(TAVI_DIR) -I$(TAVI_INCLUDE)

$(OBJ_DIR)/RecordBottomWnd.o : RecordBottomWnd.h RecordBottomWnd.cpp
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ -c RecordBottomWnd.cpp  -I$(TAVI_DIR) -I$(TAVI_INCLUDE)

$(OBJ_DIR)/RecVolCtrl.o : RecVolCtrl.h RecVolCtrl.cpp
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ -c RecVolCtrl.cpp  -I$(TAVI_DIR) -I$(TAVI_INCLUDE)

$(OBJ_DIR)/RecFolderView.o : RecFolderView.h RecFolderView.cpp
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ -c RecFolderView.cpp  -I$(TAVI_DIR) -I$(TAVI_INCLUDE)

clean:
