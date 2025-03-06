CC=gcc
# -fno-inline ; -fopt-info-vec  


CFLAGS= -Wall -O3 -ffast-math -march=native -ftree-vectorize -Winline -funroll-loops
INCLUDES=-I/opt/X11/include 
LDFLAGS=-L/opt/X11/lib -lX11 -lm 

OPENMP: ../graphics.o Makefile galsim.c
	$(CC) $(CFLAGS) -o galsim galsim.c ../graphics.o $(LDFLAGS) -fopenmp -DOPENMP

clean:
	rm galsim result.gal ../graphics.o