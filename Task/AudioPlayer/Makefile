# Makefile for Audio Player

TAVI_DIR = $(EM85xx_ROOT)/tavi
TAVI_INCLUDE = $(TAVI_DIR)/include

export OBJ_DIR CC CFLAGS LDFLAGS TAVI_DIR TAVI_INCLUDE

KERNELMODULEDIR := $(EM85xx_ROOT)/kernelmodule/realmagichwl_kernelland
KERNELUSERDIR := $(EM85xx_ROOT)/kernelmodule/realmagichwl_userland
MP3DIR := $(EM85xx_ROOT)/libraries/libmad
MP3LIB  = $(MP3DIR)/libmad.a
OGGDIR = $(EM85xx_ROOT)/thirdparties/libtremor
OGGLIB = $(OGGDIR)/libtremor.a
WMADIR = $(EM85xx_ROOT)/libraries/libwma
WMALIB = $(WMADIR)/libwma.a
RUALIB	= $(KERNELUSERDIR)/realmagichwl_userland_api.o
EQUALIZERDIR = $(EM85xx_ROOT)/libraries/libEqualizer
LIBEQUALIZER = $(EQUALIZERDIR)/equalizer.a
MELONDIR = $(EM85xx_ROOT)/libraries/libmelon
MELONLIB = $(MELONDIR)/libmelon.a
MELONOBJ = ./Melon/MelonIO.o ./Melon/Melon.o
AUDIO_LIBS    = $(MP3LIB) $(OGGLIB) $(WMALIB) $(RUALIB) $(MELONLIB)
FATFS_LIB = $(EM85xx_ROOT)/libraries/libfatfs/fatfs.a

COMPONENT_DIR := $(TAVI_DIR)/Component

AUDIO_CFLAGS := -O2 -fomit-frame-pointer -fexpensive-optimizations
AUDIO_CFLAGS += $(CFLAGS)
AUDIO_CFLAGS += -I$(KERNELMODULEDIR) -I$(KERNELUSERDIR) -I$(TAVI_DIR)/mp3 -I$(MP3DIR) -I$(OGGDIR) -I$(WMADIR) -I$(EQUALIZERDIR) -I$(WMADIR)/decoder
AUDIO_CFLAGS += -I$(MELONDIR)

CXX = arm-elf-g++

SUB_DIR = ID3 Melon

.PHONY: $(SUB_DIR) 

AUDIO_OBJS := $(TAVI_DIR)/mp3.o $(TAVI_DIR)/em85xx.o $(TAVI_DIR)/jasperlib.o wma.o $(TAVI_DIR)/ogg.o

all :  $(SUB_DIR)  $(OBJ_DIR)/AudioPlayer.o $(OBJ_DIR)/PlayListView.o $(OBJ_DIR)/FolderView.o $(OBJ_DIR)/ID3View.o \
       $(OBJ_DIR)/PlayerWnd.o  $(OBJ_DIR)/VolProgressBar.o $(OBJ_DIR)/FolderPL.o $(OBJ_DIR)/Playlist.o\
       $(OBJ_DIR)/PlayBottomWnd.o $(OBJ_DIR)/MusicDB.o $(OBJ_DIR)/MusicDBPL.o $(OBJ_DIR)/PlaylistPL.o\
       $(OBJ_DIR)/PointerList.o \
       $(OBJ_DIR)/MusicMainWnd.o MusicPlayer
       
$(SUB_DIR):
	$(MAKE) -C $@ CC=arm-elf-gcc AR=arm-elf-ar 
	
$(OBJ_DIR)/AudioPlayer.o : AudioPlayer.h AudioPlayer.cpp
	$(CXX) $(CFLAGS) -o $@ -c AudioPlayer.cpp  -I$(TAVI_DIR) -I$(TAVI_INCLUDE)

$(OBJ_DIR)/PlayListView.o : PlayListView.h PlayListView.cpp
	$(CXX) $(CFLAGS) -o $@ -c PlayListView.cpp  -I$(TAVI_DIR) -I$(TAVI_INCLUDE)

$(OBJ_DIR)/FolderView.o : FolderView.h FolderView.cpp
	$(CXX) $(CFLAGS) -o $@ -c FolderView.cpp  -I$(TAVI_DIR) -I$(TAVI_INCLUDE)

$(OBJ_DIR)/PlayBottomWnd.o : PlayBottomWnd.h PlayBottomWnd.cpp
	$(CXX) $(CFLAGS) -o $@ -c PlayBottomWnd.cpp  -I$(TAVI_DIR) -I$(TAVI_INCLUDE)
	
$(OBJ_DIR)/ID3View.o: ID3View.h ID3View.cpp
	$(CXX) $(CFLAGS) -o $@ -c ID3View.cpp  -I$(TAVI_DIR) -I$(TAVI_INCLUDE)

$(OBJ_DIR)/PlayerWnd.o: PlayerWnd.h  Playlist.h PlayerWnd.cpp
	$(CXX) $(CFLAGS) -o $@ -c PlayerWnd.cpp  -I$(TAVI_DIR) -I$(TAVI_INCLUDE)
	
$(OBJ_DIR)/VolProgressBar.o: VolProgressBar.cpp VolProgressBar.h
	$(CC) $(CFLAGS)  -o $@ -c VolProgressBar.cpp  -I$(TAVI_DIR) -I$(TAVI_INCLUDE)

MP3HeaderInfo.o: MP3HeaderInfo.cpp MP3HeaderInfo.h
	$(CC) $(CFLAGS) -o $@ -c MP3HeaderInfo.cpp  -I$(TAVI_DIR) -I$(TAVI_INCLUDE)

$(OBJ_DIR)/MusicMainWnd.o: MusicMainWnd.cpp MusicMainWnd.h
	$(CC) $(CFLAGS) -o $@ -c MusicMainWnd.cpp  -I$(TAVI_DIR) -I$(TAVI_INCLUDE)

$(OBJ_DIR)/MusicDB.o: MusicDB.cpp MusicDB.h
	$(CC) $(CFLAGS) -o $@ -c MusicDB.cpp  -I$(TAVI_DIR) -I$(TAVI_INCLUDE)

$(OBJ_DIR)/PointerList.o: PointerList.cpp PointerList.h
	$(CC) $(CFLAGS) -o $@ -c PointerList.cpp  -I$(TAVI_DIR) -I$(TAVI_INCLUDE)

$(OBJ_DIR)/PlaylistPL.o: PlaylistPL.cpp PlaylistPL.h Playlist.h
	$(CC) $(CFLAGS) -o $@ -c PlaylistPL.cpp  -I$(TAVI_DIR) -I$(TAVI_INCLUDE)

$(OBJ_DIR)/FolderPL.o: FolderPL.cpp FolderPL.h Playlist.h
	$(CC) $(CFLAGS) -o $@ -c FolderPL.cpp  -I$(TAVI_DIR) -I$(TAVI_INCLUDE)

$(OBJ_DIR)/MusicDBPL.o: MusicDBPL.cpp MusicDBPL.h Playlist.h
	$(CC) $(CFLAGS) -o $@ -c MusicDBPL.cpp  -I$(TAVI_DIR) -I$(TAVI_INCLUDE)
	
$(OBJ_DIR)/Playlist.o: Playlist.cpp Playlist.h
	$(CC) $(CFLAGS) -o $@ -c Playlist.cpp  -I$(TAVI_DIR) -I$(TAVI_INCLUDE)

MusicPlayer: MusicPlayer.o wma.o MP3HeaderInfo.o $(AUDIO_OBJS) $(AUDIO_LIBS) $(FATFS_LIB) $(MELONOBJ)
	$(CC) -s -r -nostdlib -o $@_x.o MP3HeaderInfo.o $(MELONOBJ) $(AUDIO_OBJS) $(AUDIO_LIBS) $(COMPONENT_DIR)/String.o $(COMPONENT_DIR)/FileUtil.o $(FATFS_LIB) \
	$(OBJ_DIR)/LibWideString.o $(OBJ_DIR)/tavi_mq.o ID3/libid3.o ID3/charset.o
	arm-elf-strip -g -S -X -x $@_x.o
	$(CC) $(LDFLAGS) -o $@ MusicPlayer.o $@_x.o
	cp MusicPlayer $(EM85xx_ROOT)/romfs/bin

MusicPlayer.o: MusicPlayer.cpp 
	$(CC) $(AUDIO_CFLAGS) -I$(TAVI_DIR) -I$(EM85xx_ROOT)/libraries/libfatfs -I$(EM85xx_ROOT)/pbi_ide -c MusicPlayer.cpp  

wma.o: $(TAVI_DIR)/wma/wma.cpp
	$(CC) $(CFLAGS) -c -o wma.o $(TAVI_DIR)/wma/wma.cpp -I$(WMADIR) -I$(WMADIR)/decoder

$(TAVI_DIR)/ogg.o: $(TAVI_DIR)/ogg/ogg.cpp
	$(CC) $(CFLAGS) -c -o $@ $(TAVI_DIR)/ogg/ogg.cpp -I$(OGGDIR) 

clean:
	$(MAKE) -C ID3 clean
	$(MAKE) -C Melon clean
	rm -f *.o *.gdb
