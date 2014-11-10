ifneq ("$(wildcard config.inc)","")
include config.inc
else
$(error Please run config.sh)
endif


CC = g++

CFLAGS =  -O3 -g3
H5PFLAGS = -I${HDF5root}/include -I${MPIroot}/include -I${BOOST}/include

LIB = -L. -lm -ldl
H5PLIB = -L${HDF5root}/lib -lhdf5 -L${BOOST}/lib

ADD_FLAGS =

all: replayer

H5replay.o: H5replay.cpp
	$(CC) -c -o $@ $< $(CFLAGS) $(H5PFLAGS) $(ADD_FLAGS)

replayer.o: replayer.cpp
	$(CC) -c -o $@ $< $(CFLAGS) $(H5PFLAGS) $(ADD_FLAGS)

replayer: replayer.o H5replay.o
	$(CC) -o $@ $< H5replay.o $(CFLAGS) $(H5PLIB)
	rm replayer.o

clean:
	rm -f *.o replayer
