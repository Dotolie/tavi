# $Id: Makefile,v 1.2 2006/02/22 13:06:13 zstein Exp $

#
# Or, if you don't want maker note modules...
#
NOMKRS=makers_stub.o

#
# A few parameters...
#
OBJS=e_log.o e_log10.o e_exp.o e_pow.o s_floor.o s_isnan.o e_scalb.o e_sqrt.o s_fabs.o s_scalbn.o \
		s_copysign.o math.o
HDRS=math.h math_private.h
TARGET := $(OBJ_DIR)/libmath.a

.SUFFIXES: .o .c
.c.o:
	$(CC) $(CFLAGS) -o $@ -c $<


all: $(TARGET)

$(TARGET): $(OBJS) $(HDRS)
	$(AR) rcv $(OBJ_DIR)/libmath.a $(OBJS) $(MKRS)

