# Makefile for FileManager

TAVI_DIR = $(EM85xx_ROOT)/tavi
TAVI_INCLUDE = $(TAVI_DIR)/include

export OBJ_DIR CC CFLAGS LDFLAGS TAVI_DIR TAVI_INCLUDE


all : $(OBJ_DIR)/VideoPlayer.o

$(OBJ_DIR)/VideoPlayer.o : VideoPlayer.h VideoPlayer.cpp
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ -c VideoPlayer.cpp  -I$(TAVI_DIR) -I$(TAVI_INCLUDE)

