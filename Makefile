# -- Configurable
#CDEFINES=-O1 -DMACRO_SINCOS -DSGDP4_SNGL
CDEFINES=-O2 -DMACRO_SINCOS
#CDEFINES=-O2

# -- Linux
CC=mpicc
CFLAGS=-DLINUX $(CDEFINES)
XLIBS=

RM=rm -f
AR=ar cr
RANLIB=ranlib
DEPFLAGS=-I/usr/include

LIBDIR=sgp_lib

SRCS=${LIBDIR}/aries.c ${LIBDIR}/deep.c ${LIBDIR}/ferror.c ${LIBDIR}/satutl.c ${LIBDIR}/sgdp4.c lib/ReadDB.c

OBJS=${SRCS:.c=.o}
LIB=-lm $(XLIBS)

all: main test compvec

main: $(OBJS) main.c
	$(CC) $(CFLAGS) $(DEPFLAGS) -o $@ $(OBJS) main.c $(LIB) -lm

main-seq: $(OBJS) main_sequential.c
	$(CC) $(CFLAGS) $(DEPFLAGS) -o $@ $(OBJS) main_sequential.c $(LIB) -lm

test: $(OBJS) test1.c
	$(CC) $(CFLAGS) $(DEPFLAGS) -o $@ $(OBJS) test1.c $(LIB) -lm

compvec: ${LIBDIR}/comp.c
	$(CC) $(CFLAGS) $(DEPFLAGS) -o $@ sgp_lib/comp.c $(LIB)

clean:
	$(RM) core main main.o test test.o $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) $(DEPFLAGS) -c $< -o $@