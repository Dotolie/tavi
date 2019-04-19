# Makefile for Audio Player

TAVI_DIR = $(EM85xx_ROOT)/tavi
TAVI_INCLUDE = $(TAVI_DIR)/include

export OBJ_DIR CC CFLAGS LDFLAGS TAVI_DIR TAVI_INCLUDE

SQLITEDIR = $(EM85xx_ROOT)/libraries/libsqlite3
SQLITELIB = $(SQLITEDIR)/libsqlite3.a

OBJS = 

all : testsql1 testsql2
	
sqlite1.o: sqlite1.cpp sqlite1.h
	$(CC) $(LDFLAGS) -o $@ $<

testsql1: testsql1.o $(SQLITELIB) 
	$(CC) $(LDFLAGS) -o $@ testsql1.o $(OBJS) $(SQLITELIB)
	cp testsql1 $(EM85xx_ROOT)/romfs/bin

testsql1.o: testsql1.cpp
	$(CC) $(CFLAGS) -I$(TAVI_DIR) -I$(SQLITEDIR) -c testsql1.cpp  


testsql2: testsql2.o $(SQLITELIB) 
	$(CC) $(LDFLAGS) -o $@ testsql2.o $(OBJS) $(SQLITELIB)
	cp testsql2 $(EM85xx_ROOT)/romfs/bin

testsql2.o: testsql2.cpp
	$(CC) $(CFLAGS) -I$(TAVI_DIR) -I$(SQLITEDIR) -c testsql2.cpp  

clean:
	rm -f *.o *.gdb *.elf *.elf2flt
