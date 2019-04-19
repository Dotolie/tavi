# Makefile for FileManager

TAVI_DIR = $(EM85xx_ROOT)/tavi
TAVI_INCLUDE = $(TAVI_DIR)/include

export OBJ_DIR CC CFLAGS LDFLAGS TAVI_DIR TAVI_INCLUDE


all : $(OBJ_DIR)/FileManager.o

$(OBJ_DIR)/FileManager.o : FileManager.h FileManager.cpp
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ -c FileManager.cpp  -I$(TAVI_DIR) -I$(TAVI_INCLUDE)

