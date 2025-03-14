#pragma once
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#define Cell int8_t
#define IS_EMPTY(cell) ((cell) == 0)

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

typedef struct EmptyChain_struct EmptyChain;
typedef struct CellPossibilities_struct CellPossibilities;
typedef struct board_struct
{
    int8_t base;
    int8_t side;
    Cell *cells;

    int num_empty;
    EmptyChain *empty_chain;
    int num_initial_empty;
    EmptyChain **initial_empty;

    unsigned     char **rows;
    unsigned char **cols;
    unsigned char **boxes;
} Board;

struct EmptyChain_struct
{
    int idx;
    struct EmptyChain_struct *next;
};

void print_ec(EmptyChain *ec)
{
    printf("Empty chain: ");
    while (ec != NULL)
    {
        printf("%d ", ec->idx);
        ec = ec->next;
    }
    printf("\n");
}

inline Cell get_cell(Board *b, int x, int y)
{
    return b->cells[y * b->side + x];
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
    for (int i = 0; i < b->num_initial_empty; i++)
    {
        free(b->initial_empty[i]);
    }
    free(b->initial_empty);
    free(b);
}
