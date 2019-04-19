# Makefile for TextViewer

TAVI_DIR = $(EM85xx_ROOT)/tavi
TAVI_INCLUDE = $(TAVI_DIR)/include

export OBJ_DIR CC CFLAGS LDFLAGS TAVI_DIR TAVI_INCLUDE


all : $(OBJ_DIR)/TextViewer.o

$(OBJ_DIR)/TextViewer.o : TextViewer.h TextViewer.cpp
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ -c TextViewer.cpp  -I$(TAVI_DIR) -I$(TAVI_INCLUDE)

