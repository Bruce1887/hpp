#pragma once
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#define Cell int8_t
#define Mask int64_t
#define MASK_BITSIZE (sizeof(Mask) * 8)
#define VACANT 0
#define OCCUPIED 1
#define IS_EMPTY(cell) ((cell) == VACANT)

#ifdef DEBUG
#include <assert.h>
#define DEBUG_ASSERT(cond) assert(cond)
#define DEBUG_PRINT(print_statement) print_statement
#else
#define DEBUG_ASSERT(cond) ((void)0)
#define DEBUG_PRINT(cond) ((void)0);
#endif


typedef struct board_struct
{
    int8_t base;
    int8_t side;
    Cell *cells;

    // Masks for rows, columns and boxes. Used to check for duplicates
    int cell_mask_size;
    Mask **c_mask;
    Mask **r_mask;
    Mask **b_mask;

    // Mask for empty cells. Used to find the next empty cell
    int empty_mask_size;
    Mask *empty_mask;
    int num_empty;
} Board;

Cell get_cell(Board *b, int row, int col);

// gets top left cell of box
int get_box_idx(Board *b, int row, int col);

void print_mask(Mask *m, int m_size);

int get_first_empty(Board *b);


void update_mask(Mask *mask, int idx, bool val);

void update_cell_masks(Board *b, int x, int y, int val, bool set);

void get_coords(Board *b, int idx, int *x, int *y);

int get_index(Board *b, int x, int y);

void print_board(Board *b);