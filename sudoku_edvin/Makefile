CC := gcc
CFLAGS := -Wall -O3 -ffast-math -march=native -ftree-vectorize -Winline -funroll-loops -fopenmp	
INCLUDES := -I/opt/X11/include
LDFLAGS := -L/opt/X11/lib -lX11 -lm

SRC := solver.c printer.c
OBJ := $(SRC:.c=.o)
DEPS := solver.h parser.h validator.h Makefile

all: solver

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

solver: solver.o
	$(CC) $(CFLAGS) -o $@ solver.o $(LDFLAGS)

printer: printer.o
	$(CC) $(CFLAGS) -o $@ printer.o $(LDFLAGS)

debug: CFLAGS += -g -DDEBUG
debug: solver

clean:
	rm -f solver printer *.o output.dat


# -fno-inline ; -fopt-info-vec  ; 