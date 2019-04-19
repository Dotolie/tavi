OBJS = libid3.o charset.o m3u.o WMATag.o OGGTag.o
FATFS_DIR = $(EM85xx_ROOT)/libraries/libfatfs
TAVI_DIR = $(EM85xx_ROOT)/tavi
JASPER_DRV = $(EM85xx_ROOT)/kernelmodule/realmagichwl_kernelland
JASPER_USR = $(EM85xx_ROOT)/kernelmodule/realmagichwl_userland

INC_PATH = -I$(FATFS_DIR) -I$(TAVI_DIR) -I$(TAVI_DIR)/include
#INC_PATH += -I$(TAVI_DIR)/stlport -I$(TAVI_DIR)/stlport/stl
all: $(OBJS) 
	cp $(OBJS) $(OBJ_DIR)

#.c.o:
#	$(CC) -c  $(INC_PATH) $<

.cpp.o: 
	$(CC) -c  $(INC_PATH) -I$(JASPER_DRV) -I$(JASPER_USR) $<

clean:
	rm -f *.o
