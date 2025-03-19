/**
 * @file solver.c
 * @author Edvin Bruce (student id edbr8688)
 * @brief Project in HPP Spring 2025
 * @date 2025-03-10
 */

#include <stdio.h>
#include <time.h>

#include "parser.h"
#include "solver.h"
#include "validator.h"

void usage(char *program_name)
{
    printf("Usage: %s <filename> \n", program_name);
    exit(EXIT_FAILURE);
}

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
bool solve_my_board(Board *b)
{
    if (b->num_empty == 0)
        return true;

    // DEBUG_ASSERT(b->num_empty > 47);
    int idx = get_first_empty(b);
    DEBUG_PRINT(printf("\n### NEW EMPTY, idx; %d ###\n", idx));
    DEBUG_ASSERT(idx != -1);
    DEBUG_ASSERT(idx < b->side * b->side);
    DEBUG_PRINT(printf("b->cells[%d]: %d\n", idx, b->cells[idx]));
    DEBUG_ASSERT(b->cells[idx] == 0);
    DEBUG_PRINT(printf("b->num_empty: %d\n", b->num_empty));

    // try all possible values in that cell
    for (int val = 1; val <= b->side; val++)
    {
        b->cells[idx] = val;
        update_mask(b->empty_mask, idx + 1, OCCUPIED); // this "idx +1 " is wonky, but it works
        b->num_empty--;

        int row;
        int col;
        get_coords(b, idx, &row, &col);

        if (validate_update(b, row, col, val))
        {
            DEBUG_PRINT(print_board(b));
            update_cell_masks(b, row, col, val, OCCUPIED);
            if (solve_my_board(b))
            {
                return true;
            }
            else
            {
                // undo the update
                DEBUG_PRINT(printf("Undoing update at (%d, %d) with value %d\n", row, col, val));
                update_cell_masks(b, row, col, val, VACANT);
            }
        }
        update_mask(b->empty_mask, idx + 1, VACANT); // this "idx +1 " is wonky, but it works
        b->num_empty++;
        b->cells[idx] = VACANT;
    }

    b->cells[idx] = 0;
    return false;
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        usage(argv[0]);
    }

#ifdef _OPENMP
    printf("OpenMP version: %d\n", _OPENMP);
#endif

    Board *b = read_dat_file(argv[1], 0);
    printf("base: %d\n", b->base);
    printf("side: %d\n", b->side);

    printf("board num empty: %d\n", b->num_empty);
    print_board(b);
    printf("\nStarting timer...\n");
    clock_t start_time = clock();
    bool solution_found = solve_my_board(b);
    clock_t end_time = clock();
    double time_spent = (double)(end_time - start_time) / CLOCKS_PER_SEC;

    printf("Time taken to solve the board: %f seconds\n", time_spent);

    printf("Board is %s\n", solution_found ? "solved" : "not solved");
    print_board(b);
    if (solution_found)
    {
        assert(validate_board(b));
        printf("Board is valid\n");
    }

    write_board_to_file(b);

    delete_board(b);
    exit(EXIT_SUCCESS);
}