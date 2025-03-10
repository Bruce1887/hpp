#include <stdio.h>
#include "solver.h"
#include "parser.h"

int main(){
    int base = 3;
    int side = 3;

    Board *b = malloc(sizeof(Board));
    b->base = base;
    b->side = side;
    b->cells = (Cell[]){1, 2, 3, 4, 5, 6, 7, 8, 1};

    write_board_to_file(b);
    free(b);
    return 0;
}