# Copyright(C) 2004, 2005 NewMediaLife Inc.
#
# Makefile for Components
#
# 
# $Id: Makefile,v 1.2 2006/03/09 03:04:30 zstein Exp $
# $Author: zstein $ Lee Seok<zstein@newmedialife.com>
# $Date: 2006/03/09 03:04:30 $
# 
# $Log: Makefile,v $
# Revision 1.2  2006/03/09 03:04:30  zstein
# *** empty log message ***
#
# Revision 1.1.1.1  2006/02/22 12:33:31  zstein
# TAVI020 REBUILD
#
# Revision 1.14  2005/10/18 11:29:00  zstein
# - deleted TouchMenu.o
#
# Revision 1.13  2005/07/16 04:13:51  zstein
# - fixed bugs & added functions.
#
# Revision 1.12  2005/05/15 11:08:49  zstein
# *** empty log message ***
#
# Revision 1.11  2005/05/12 04:18:32  zstein
# *** empty log message ***
#
# Revision 1.10  2005/04/27 15:45:14  ywkim
# add instruction for ClockSetView.cpp
#
# Revision 1.9  2005/04/05 08:02:36  zstein
# *** empty log message ***
#
# Revision 1.8  2005/03/29 03:33:57  ywkim
# Add slidebar class
#
# Revision 1.7  2005/03/01 02:11:05  zstein
# *** empty log message ***
#
#

TAVI_DIR = $(EM85xx_ROOT)/tavi

CC = arm-elf-gcc
CXX = arm-elf-g++

LDFLAGS := 

srcs_cxx := fbdev.cpp Window.cpp image.cpp timer.cpp ListView.cpp Pixmap.cpp Label.cpp \
		RadioButton.cpp String.cpp ProgressBar.cpp FileUtil.cpp VScrollBar.cpp \
		SpinSelectBar.cpp ComplexLabel.cpp ComplexListView.cpp ModalWnd.cpp BottomWnd.cpp \
		Bitmap.cpp SlideBar.cpp ClockSetView.cpp CharSet.cpp ProgressDlg.cpp ContextMenu.cpp

target_o := $(OBJ_DIR)/components.o

-include $(EM85xx_ROOT)/Rules.mk


-include .depend



