# -- Configurable
#CDEFINES=-O1 -DMACRO_SINCOS -DSGDP4_SNGL
CDEFINES=-O2 -DMACRO_SINCOS
#CDEFINES=-O2

# -- Linux
CC=gcc
CFLAGS=-Wall -DLINUX $(CDEFINES)
XLIBS=

RM=rm -f
AR=ar cr
RANLIB=ranlib
DEPFLAGS=-I/usr/include

SRCS=aries.c deep.c ferror.c satutl.c sgdp4.c test1.c

OBJS=${SRCS:.c=.o}
LIB=-lm $(XLIBS)

all: testsgp compvec

testsgp: $(OBJS)
	$(CC) $(CFLAGS) $(DEPFLAGS) -o $@ $(OBJS) $(LIB)

compvec: comp.c
	$(CC) $(CFLAGS) $(DEPFLAGS) -o $@ comp.c $(LIB)

clean:
	$(RM) core $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) $(DEPFLAGS) -c $*.c