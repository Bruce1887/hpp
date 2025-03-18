#pragma once
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#define Cell int8_t
#define Mask int64_t
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

void usage(char *program_name)
{
    printf("Usage: %s <filename> \n", program_name);
    exit(EXIT_FAILURE);
}

typedef struct board_struct
{
    int8_t base;
    int8_t side;
    Cell *cells;

    Mask **c_mask;
    Mask **r_mask;
    Mask **b_mask;
    int num_boxes;

    int mask_size;
    Mask *empty_mask;
    int num_empty;
} Board;

inline Cell get_cell(Board *b, int x, int y)
{
    return b->cells[y * b->side + x];
}

void print_mask(Board *b)
{
    for (int i = 0; i < b->mask_size; i++)
    {
        for (int j = 63; j >= 0; j--)
        { // Print from MSB to LSB
            printf("%ld", (b->empty_mask[i] >> j) & 1);
        }
        printf(" "); // Space between each 64-bit chunk
    }
    printf("\n");
}

inline int get_first_empty(Board *b)
{
    for (int i = 0; i < b->mask_size; i++)
    {
        if (~b->empty_mask[i])
        { // If there is at least one 0-bit
            return i * 64 + __builtin_ctzll(~b->empty_mask[i]);
        }
    }
    return -1; // No vacant cells found
}

inline void update_mask(Mask *mask, int idx, bool val)
{
    DEBUG_ASSERT(idx >= 0 && idx < b->side * b->side);
    if (val)
        mask[idx / 64] |= ((uint64_t)1 << (idx % 64)); // Set bit to 1
    else
        mask[idx / 64] &= ~((uint64_t)1 << (idx % 64)); // Clear bit (set to 0)
}

inline void get_coords(Board *b, int idx, int *x, int *y)
{
    DEBUG_ASSERT(idx < b->side * b->side);
    *x = idx % b->side;
    *y = idx / b->side;
    DEBUG_ASSERT(*x < b->side);
    DEBUG_ASSERT(*y < b->side);
    DEBUG_ASSERT(*x >= 0);
    DEBUG_ASSERT(*y >= 0);
}

inline int get_index(Board *b, int x, int y)
{
    DEBUG_ASSERT(x < b->side);
    DEBUG_ASSERT(y < b->side);
    DEBUG_ASSERT(x >= 0);
    DEBUG_ASSERT(y >= 0);
    return y * b->side + x;
}

void print_board(Board *b)
{
    for (int i = 0; i < b->side; i++)
    {
        printf("##");
    }
    printf("\n");
    for (int i = 0; i < b->side; i++)
    {
        for (int j = 0; j < b->side; j++)
        {
            printf("%d ", b->cells[i * b->side + j]);
        }
        printf("\n");
    }
    for (int i = 0; i < b->side; i++)
    {
        printf("##");
    }
    printf("\n");
}

void delete_board(Board *b)
{
    free(b->cells);
    free(b->empty_mask);
    free(b);
}
