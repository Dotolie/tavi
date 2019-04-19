# Makefile for Lib

TAVI_DIR = $(EM85xx_ROOT)/tavi
TAVI_INCLUDE = $(TAVI_DIR)/include

all :  $(OBJ_DIR)/LibUtilities.o \
	$(OBJ_DIR)/LibImage.o \
	$(OBJ_DIR)/LibScrollBar.o \
	$(OBJ_DIR)/LibDialogBox.o \
	$(OBJ_DIR)/LibBookmarkDialogBox.o \
	$(OBJ_DIR)/LibSetupDialogBox.o \
	$(OBJ_DIR)/LibControlPannel.o \
	$(OBJ_DIR)/LibListBox.o \
	$(OBJ_DIR)/LibFlash.o \
	$(OBJ_DIR)/LibFileBrowser.o \
	$(OBJ_DIR)/LibBuzzer.o \
	$(OBJ_DIR)/LibWideString.o \
	$(OBJ_DIR)/LibText.o \
	$(OBJ_DIR)/LibMsgBox.o \
	$(OBJ_DIR)/LibSubtitle.o \
	$(OBJ_DIR)/LibSubMenu.o \
	$(OBJ_DIR)/LibFileDbx.o \
	$(OBJ_DIR)/LibVirtualKeyboard.o \
	$(OBJ_DIR)/LibSoundPaletteDbx.o \
	$(OBJ_DIR)/LibSubtitleSyncDbx.o \
	$(OBJ_DIR)/LibNFile.o 

$(OBJ_DIR)/LibUtilities.o : LibUtilities.h LibUtilities.cpp
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ -c LibUtilities.cpp  -I$(TAVI_DIR) -I$(TAVI_INCLUDE)

$(OBJ_DIR)/LibImage.o : LibImage.h LibImage.cpp
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ -c LibImage.cpp  -I$(TAVI_DIR) -I$(TAVI_INCLUDE)

$(OBJ_DIR)/LibScrollBar.o : LibScrollBar.h LibScrollBar.cpp
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ -c LibScrollBar.cpp   -I$(TAVI_DIR) -I$(TAVI_INCLUDE)

$(OBJ_DIR)/LibDialogBox.o : LibDialogBox.h LibDialogBox.cpp
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ -c LibDialogBox.cpp   -I$(TAVI_DIR) -I$(TAVI_INCLUDE)

$(OBJ_DIR)/LibBookmarkDialogBox.o : LibBookmarkDialogBox.h LibBookmarkDialogBox.cpp
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ -c LibBookmarkDialogBox.cpp -I$(TAVI_DIR) -I$(TAVI_INCLUDE)

$(OBJ_DIR)/LibSetupDialogBox.o : LibSetupDialogBox.h LibSetupDialogBox.cpp
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ -c LibSetupDialogBox.cpp -I$(TAVI_DIR) -I$(TAVI_INCLUDE)

$(OBJ_DIR)/LibControlPannel.o : LibControlPannel.h LibControlPannel.cpp
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ -c LibControlPannel.cpp   -I$(TAVI_DIR) -I$(TAVI_INCLUDE)

$(OBJ_DIR)/LibListBox.o : LibListBox.h LibListBox.cpp
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ -c LibListBox.cpp   -I$(TAVI_DIR) -I$(TAVI_INCLUDE)

$(OBJ_DIR)/LibFlash.o : LibFlash.h LibFlash.cpp
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ -c LibFlash.cpp   -I$(TAVI_DIR) -I$(TAVI_INCLUDE)

$(OBJ_DIR)/LibFileBrowser.o : LibFileBrowser.h LibFileBrowser.cpp
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ -c LibFileBrowser.cpp   -I$(TAVI_DIR) -I$(TAVI_INCLUDE)

$(OBJ_DIR)/LibBuzzer.o : LibBuzzer.h LibBuzzer.cpp
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ -c LibBuzzer.cpp   -I$(TAVI_DIR) -I$(TAVI_INCLUDE)

$(OBJ_DIR)/LibWideString.o : LibWideString.h LibWideString.cpp
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ -c LibWideString.cpp   -I$(TAVI_DIR) -I$(TAVI_INCLUDE)

$(OBJ_DIR)/LibText.o : LibText.h LibText.cpp
	$(CC) $(CFLAGS) -fsigned-char $(LDFLAGS) -o $@ -c LibText.cpp   -I$(TAVI_DIR) -I$(TAVI_INCLUDE)

$(OBJ_DIR)/LibMsgBox.o : LibMsgBox.h LibMsgBox.cpp
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ -c LibMsgBox.cpp   -I$(TAVI_DIR) -I$(TAVI_INCLUDE)

$(OBJ_DIR)/LibSubtitle.o : LibSubtitle.h LibSubtitle.cpp
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ -c LibSubtitle.cpp   -I$(TAVI_DIR) -I$(TAVI_INCLUDE)

$(OBJ_DIR)/LibSubMenu.o : LibSubMenu.h LibSubMenu.cpp
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ -c LibSubMenu.cpp   -I$(TAVI_DIR) -I$(TAVI_INCLUDE)

$(OBJ_DIR)/LibFileDbx.o : LibFileDbx.h LibFileDbx.cpp
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ -c LibFileDbx.cpp   -I$(TAVI_DIR) -I$(TAVI_INCLUDE)

$(OBJ_DIR)/LibVirtualKeyboard.o : LibVirtualKeyboard.h LibVirtualKeyboard.cpp
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ -c LibVirtualKeyboard.cpp   -I$(TAVI_DIR) -I$(TAVI_INCLUDE)

$(OBJ_DIR)/LibSoundPaletteDbx.o : LibSoundPaletteDbx.h LibSoundPaletteDbx.cpp
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ -c LibSoundPaletteDbx.cpp   -I$(TAVI_DIR) -I$(TAVI_INCLUDE)

$(OBJ_DIR)/LibSubtitleSyncDbx.o : LibSubtitleSyncDbx.h LibSubtitleSyncDbx.cpp
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ -c LibSubtitleSyncDbx.cpp   -I$(TAVI_DIR) -I$(TAVI_INCLUDE)

$(OBJ_DIR)/LibNFile.o : LibNFile.h LibNFile.cpp
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ -c LibNFile.cpp   -I$(TAVI_DIR) -I$(TAVI_INCLUDE)
# END
