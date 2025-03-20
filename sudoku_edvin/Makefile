CC := gcc
CFLAGS := -Wall -Winline 
OPTFLAGS := -Wall -Ofast -march=native -funroll-loops -fsplit-loops -fomit-frame-pointer -finline-functions -finline-limit=1000 -fno-math-errno
INCLUDES := -I/opt/X11/include
LDFLAGS := -L/opt/X11/lib -lX11 -lm

SRC := solver.c parser.c validator.c board.c
OBJ := $(SRC:.c=.o)
DEPS := solver.h parser.h validator.h board.h Makefile

all: CFLAGS += $(OPTFLAGS)
all: solver

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

solver: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ) $(LDFLAGS)


debug: CFLAGS += -g -DDEBUG
debug: solver

profile: CFLAGS += -p
profile: solver

clean:
	rm -f solver *.o output.dat
