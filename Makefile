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

all: main-par main-seq Armaggedon

main-par: $(OBJS) main_parallel.c
	$(CC) $(CFLAGS) $(DEPFLAGS) -o $@ $(OBJS) main_parallel.c $(LIB) -lm

main-seq: $(OBJS) main_sequential.c
	$(CC) $(CFLAGS) $(DEPFLAGS) -o $@ $(OBJS) main_sequential.c $(LIB) -lm

Armaggedon: Armaggedon.c
	$(CC) ./lib/ReadDB.c ./lib/Astrodynamics.c Armaggedon.c -o $@ -lm

compvec: ${LIBDIR}/comp.c
	$(CC) $(CFLAGS) $(DEPFLAGS) -o $@ sgp_lib/comp.c $(LIB)

test: $(OBJS) ${LIBDIR}/test1.c
	$(CC) $(CFLAGS) $(DEPFLAGS) -o $@ $(OBJS) sgp_lib/test1.c $(LIB) -lm

clean:
	$(RM) ${LIBDIR}/*.o *.o $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) $(DEPFLAGS) -c $< -o $@