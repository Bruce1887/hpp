CC=gcc
CFLAGS=-Wall
INCLUDES=-I/opt/X11/include
LDFLAGS=-L/opt/X11/lib -lX11 -lm

galsim: galsim.o galsim.h graphics/graphics.o 
	$(CC) -o galsim galsim.o graphics/graphics.o $(LDFLAGS)

clean:
	rm galsim *.o
