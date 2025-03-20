#include <time.h>
#include <stdio.h>

#include "parser.h"
#include "validator.h"
#include "board.h"
void delete_masks(Board *b)
{
    for (int i = 0; i < b->side; i++)
    {
        free(b->r_mask[i]);
        free(b->c_mask[i]);
        free(b->b_mask[i]);
    }
    free(b->r_mask);
    free(b->c_mask);
    free(b->b_mask);

    free(b->empty_mask);
}
void delete_board(Board *b)
{
    free(b->cells);
    delete_masks(b);
    free(b);
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

Cell get_cell(Board *b, int row, int col)
{
    return b->cells[row * b->side + col];
}

int get_box_idx(Board *b, int row, int col)
{
    int box_r = row - row % b->base;
    int box_col = col - col % b->base;
    DEBUG_ASSERT(box_r % b->base == 0);
    DEBUG_ASSERT(box_col % b->base == 0);
    int box_idx = box_col / b->base + box_r / b->base * b->base;
    return box_idx;
}
inline void print_mask(Mask *m, int m_size)
{
    for (int i = 0; i < m_size; i++)
    {
        for (int j = MASK_BITSIZE - 1; j >= 0; j--)
        { // Print from MSB to LSB
            printf("%ld", (m[i] >> j) & 1);
        }
        printf(" "); // Space between each 64-bit chunk (64 is 8 * sizeof(Mask))
    }
    printf("\n");
}

inline int get_first_empty(Board *b)
{
    for (int i = 0; i < b->empty_mask_size; i++)
    {
        if (~b->empty_mask[i])
        { // If there is at least one 0-bit
            int empty_idx = i * sizeof(Mask) * sizeof(Mask) + __builtin_ctzll(~b->empty_mask[i]);
            DEBUG_ASSERT(empty_idx < b->side * b->side);

            return empty_idx;
        }
    }
    DEBUG_PRINT(printf("No empty cells found\n"));
    DEBUG_ASSERT(false); // should never reach here
    return -1;           // No vacant cells found
}

inline void update_mask(Mask *mask, int idx, bool val)
{
    idx--;
    // printf("idx/MASK_BITSIZE: %ld\n", idx / MASK_BITSIZE);
    if (val)
        mask[idx / MASK_BITSIZE] |= ((Mask)1 << (idx % MASK_BITSIZE)); // Set bit to 1
    else
        mask[idx / MASK_BITSIZE] &= ~((Mask)1 << (idx % MASK_BITSIZE)); // Clear bit (set to 0)
}

inline void update_cell_masks(Board *b, int row, int col, int val, bool set)
{
    update_mask(b->r_mask[row], val, set);
    update_mask(b->c_mask[col], val, set);
    update_mask(b->b_mask[get_box_idx(b, row, col)], val, set);
    DEBUG_PRINT(printf("UPDATED row: %d, col: %d, box: %d\n", row, col, get_box_idx(b, row, col)));

    DEBUG_ASSERT(duplicate_in_row(b, row, val) == set);
    DEBUG_ASSERT(duplicate_in_col(b, col, val) == set);
    DEBUG_ASSERT(duplicate_in_box(b, row, col, val) == set);
}

inline void get_coords(Board *b, int idx, int *row, int *col)
{
    DEBUG_ASSERT(idx < b->side * b->side);
    *row = idx / b->side;
    *col = idx % b->side;
    DEBUG_ASSERT(*row < b->side);
    DEBUG_ASSERT(*col < b->side);
    DEBUG_ASSERT(*row >= 0);
    DEBUG_ASSERT(*col >= 0);
}

inline int get_index(Board *b, int x, int y)
{
    DEBUG_ASSERT(x < b->side);
    DEBUG_ASSERT(y < b->side);
    DEBUG_ASSERT(x >= 0);
    DEBUG_ASSERT(y >= 0);
    return y * b->side + x;
}

Board* deep_copy_board(const Board* src) {
    Board* dst = malloc(sizeof(Board));
    if (!dst) return NULL;

    // Copy primitive types
    dst->base = src->base;
    dst->side = src->side;
    dst->cell_mask_size = src->cell_mask_size;
    dst->empty_mask_size = src->empty_mask_size;
    dst->num_empty = src->num_empty;

    // Deep copy cells
    dst->cells = malloc(sizeof(Cell) * src->side * src->side);
    if (!dst->cells) { free(dst); return NULL; }
    memcpy(dst->cells, src->cells, sizeof(Cell) * src->side * src->side);

    // Deep copy masks
    int mask_count = src->side; // Assuming one mask per row/column/box
    dst->c_mask = malloc(sizeof(Mask*) * mask_count);
    dst->r_mask = malloc(sizeof(Mask*) * mask_count);
    dst->b_mask = malloc(sizeof(Mask*) * mask_count);
    if (!dst->c_mask || !dst->r_mask || !dst->b_mask) {
        free(dst->cells);
        free(dst->c_mask);
        free(dst->r_mask);
        free(dst->b_mask);
        free(dst);
        return NULL;
    }

    for (int i = 0; i < mask_count; i++) {
        dst->c_mask[i] = malloc(sizeof(Mask));
        dst->r_mask[i] = malloc(sizeof(Mask));
        dst->b_mask[i] = malloc(sizeof(Mask));
        if (!dst->c_mask[i] || !dst->r_mask[i] || !dst->b_mask[i]) {
            // Free previously allocated memory on failure
            for (int j = 0; j <= i; j++) {
                free(dst->c_mask[j]);
                free(dst->r_mask[j]);
                free(dst->b_mask[j]);
            }
            free(dst->cells);
            free(dst->c_mask);
            free(dst->r_mask);
            free(dst->b_mask);
            free(dst);
            return NULL;
        }
        memcpy(dst->c_mask[i], src->c_mask[i], sizeof(Mask));
        memcpy(dst->r_mask[i], src->r_mask[i], sizeof(Mask));
        memcpy(dst->b_mask[i], src->b_mask[i], sizeof(Mask));
    }

    // Deep copy empty_mask
    dst->empty_mask = malloc(sizeof(Mask) * src->empty_mask_size);
    if (!dst->empty_mask) {
        for (int i = 0; i < mask_count; i++) {
            free(dst->c_mask[i]);
            free(dst->r_mask[i]);
            free(dst->b_mask[i]);
        }
        free(dst->cells);
        free(dst->c_mask);
        free(dst->r_mask);
        free(dst->b_mask);
        free(dst);
        return NULL;
    }
    memcpy(dst->empty_mask, src->empty_mask, sizeof(Mask) * src->empty_mask_size);

    return dst;
}