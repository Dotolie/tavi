# Makefile for ImageViewer

TAVI_DIR = $(EM85xx_ROOT)/tavi
TAVI_INCLUDE = $(TAVI_DIR)/include

LDFLAGS := 

subdirs := exiftags-1.00

srcs_c :=
srcs_cxx := ImageViewer.cpp IVMainWnd.cpp IVListView.cpp IVSListView.cpp IVPreView.cpp \
			IVTileViewer.cpp IViewer.cpp IViewerBottomWnd.cpp IVInfoWnd.cpp ThumbDB.cpp


target_o := $(OBJ_DIR)/photo.o

-include $(EM85xx_ROOT)/Rules.mk

-include .depend
