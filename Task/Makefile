# Makefile for TASK

SUB_DIR = AudioPlayer VideoPlayer TextViewer ImageViewer FMRadio FileManager setup Recorder Extras
TAVI_DIR = $(EM85xx_ROOT)/tavi
TAVI_INCLUDE = $(TAVI_DIR)/include

.PHONY : $(SUB_DIR) all clear

#export OBJ_DIR  CC  CFLAGS



all : $(OBJ_DIR)/Task.o 	$(OBJ_DIR)/Msg.o $(OBJ_DIR)/tavi_global.o \
		$(OBJ_DIR)/SndCtrl.o $(OBJ_DIR)/TopPanel.o $(OBJ_DIR)/MiscCtrl.o $(OBJ_DIR)/SubLCD.o \
		$(OBJ_DIR)/tavi_mq.o $(SUB_DIR) $(OBJ_DIR)/ExtLCD.o $(OBJ_DIR)/MP3Rec.o

$(SUB_DIR):
	cd $@ ; $(MAKE) all

$(OBJ_DIR)/Task.o : Task.h Task.cpp
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ -c Task.cpp -I$(TAVI_DIR) -I$(TAVI_INCLUDE)
	arm-elf-strip -g -S -X -x $(OBJ_DIR)/Task.o

$(OBJ_DIR)/Msg.o : Msg.h Msg.cpp
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ -c Msg.cpp  -I$(TAVI_DIR) -I$(TAVI_INCLUDE)
	arm-elf-strip -g -S -X -x $(OBJ_DIR)/Msg.o

$(OBJ_DIR)/tavi_global.o: tavi_global.h tavi_global.cpp ../version.h
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ -c tavi_global.cpp -I$(TAVI_DIR) -I$(TAVI_INCLUDE)
	arm-elf-strip -g -S -X -x $(OBJ_DIR)/tavi_global.o

$(OBJ_DIR)/SndCtrl.o: SndCtrl.h SndCtrl.cpp
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ -c SndCtrl.cpp -I$(TAVI_DIR) -I$(TAVI_INCLUDE)
	arm-elf-strip -g -S -X -x $(OBJ_DIR)/SndCtrl.o

$(OBJ_DIR)/TopPanel.o: TopPanel.h TopPanel.cpp
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ -c TopPanel.cpp -I$(TAVI_DIR) -I$(TAVI_INCLUDE)
	arm-elf-strip -g -S -X -x $(OBJ_DIR)/TopPanel.o

$(OBJ_DIR)/MiscCtrl.o: MiscCtrl.h MiscCtrl.cpp
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ -c MiscCtrl.cpp -I$(TAVI_DIR) -I$(TAVI_INCLUDE)
	arm-elf-strip -g -S -X -x $(OBJ_DIR)/MiscCtrl.o

$(OBJ_DIR)/SubLCD.o: SubLCD.h SubLCD.cpp
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ -c SubLCD.cpp -I$(TAVI_DIR) -I$(TAVI_INCLUDE)
	arm-elf-strip -g -S -X -x $(OBJ_DIR)/SubLCD.o

$(OBJ_DIR)/tavi_mq.o: tavi_mq.h tavi_mq.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ -c tavi_mq.c -I$(TAVI_DIR) -I$(TAVI_INCLUDE)
	arm-elf-strip -g -S -X -x $(OBJ_DIR)/tavi_mq.o

$(OBJ_DIR)/ExtLCD.o: ExtLCD.h ExtLCD.cpp
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ -c ExtLCD.cpp -I$(TAVI_DIR) -I$(TAVI_INCLUDE)
	arm-elf-strip -g -S -X -x $(OBJ_DIR)/ExtLCD.o

$(OBJ_DIR)/MP3Rec.o: MP3Rec.h MP3Rec.cpp
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ -c MP3Rec.cpp -I$(TAVI_DIR) -I$(TAVI_INCLUDE)	
	arm-elf-strip -g -S -X -x $(OBJ_DIR)/MP3Rec.o

clean:
	@for dir in $(SUB_DIR);\
	  do $(MAKE) -C $$dir clean; \
	  done

