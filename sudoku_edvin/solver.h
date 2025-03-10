#pragma once
#include <stdlib.h>
#include <stdint.h>

#define Cell int8_t
#define IS_EMPTY(cell) (cell == 0)

typedef struct board_struct
{
    int8_t base;
    int8_t side;
    Cell *cells;
} Board;

inline Cell get_cell(Board *b, int x, int y){
    return b->cells[y * b->side + x];
}

inline int num_cells(Board *b){
    return b->side * b->side;
}

void delete_board(Board *b){
    free(b->cells);
    free(b);
}

