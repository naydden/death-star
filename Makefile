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

SRCS=sgp_lib/aries.c sgp_lib/deep.c sgp_lib/ferror.c sgp_lib/satutl.c sgp_lib/sgdp4.c lib/ReadDB.c main.c

OBJS=${SRCS:.c=.o}
LIB=-lm $(XLIBS)

all: main compvec

main: $(OBJS)
	$(CC) $(CFLAGS) $(DEPFLAGS) -o $@ $(OBJS) $(LIB) -lm

compvec: sgp_lib/comp.c
	$(CC) $(CFLAGS) $(DEPFLAGS) -o $@ sgp_lib/comp.c $(LIB)

clean:
	$(RM) core $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) $(DEPFLAGS) -c $< -o $@