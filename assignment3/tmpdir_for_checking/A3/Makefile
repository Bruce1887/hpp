CC=gcc
# -fno-inline


CFLAGS=-Wall -O3 -ffast-math -march=native -ftree-vectorize -fopt-info-vec -Winline -funroll-loops -fopenmp
INCLUDES=-I/opt/X11/include 
LDFLAGS=-L/opt/X11/lib -lX11 -lm 

galsim: graphics/graphics.o Makefile galsim.c
	$(CC) $(CFLAGS) -o galsim galsim.c graphics/graphics.o $(LDFLAGS) 

clean:
	rm galsim result.gal