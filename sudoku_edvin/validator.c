#include <stdio.h>
#include "validator.h"

inline bool check_mask(Mask *mask, int value)
{
    value--;
    DEBUG_ASSERT(value >= 0);
    int idx = value / MASK_BITSIZE;
    int bit_pos = value % MASK_BITSIZE;
    return mask[idx] & (1LL << bit_pos);
}

inline bool duplicate_in_row(Board *b, int r, int value)
{
    Mask *mask = b->r_mask[r];
    return check_mask(mask, value);
}

inline bool duplicate_in_col(Board *b, int c, int value)
{
    Mask *mask = b->c_mask[c];
    return check_mask(mask, value);
}

inline bool duplicate_in_box(Board *b, int row, int col, int value)
{
    Mask *mask = b->b_mask[get_box_idx(b, row, col)];
    return check_mask(mask, value);
}

// this is only called after the sudoku board has been solved to verify the solution
bool validate_board(Board *b)
{
    for (int i = 0; i < b->side; i++)
    {
        for (int j = 0; j < b->side; j++)
        {
            Cell val = get_cell(b, i, j);
            if (IS_EMPTY(val))
            {
                DEBUG_PRINT(printf("Empty cell found at (%d, %d)\n", i, j));
                return false;
            }
            else
            {
                if (duplicate_in_row(b, j, val) == false)
                {
                    print_mask(b->r_mask[j], b->cell_mask_size);
                    DEBUG_PRINT(printf("Found valid placement of %d in row %d found at (%d, %d)\n", val, j, i, j));
                    return false;
                }
                if (duplicate_in_col(b, i, val) == false)
                {
                    print_mask(b->r_mask[i], b->cell_mask_size);
                    DEBUG_PRINT(printf("Found valid placement of %d in col %d found at (%d, %d)\n", val, i, i, j));
                    return false;
                }
                int bx = i - i % b->base;
                int by = j - j % b->base;
                if (duplicate_in_box(b, bx, by, val) == false)
                {
                    print_mask(b->b_mask[i / b->base + j / b->base * b->base], b->cell_mask_size);
                    DEBUG_PRINT(printf("Found valid placement of %d in box(%d,%d) found at (%d, %d)\n", val, bx, by, i, j));
                    return false;
                }
            }
        }
    }
    return true;
}

// TODO: add an inline here
bool validate_update(Board *b, int row, int col, int value)
{
    DEBUG_PRINT(printf("Validating update at (%d, %d) with value %d\n", row, col, value));
    // print_board(b);
    if (duplicate_in_row(b, row, value))
    {
        DEBUG_PRINT(printf("Found duplicate of %d in row %d\n", value, row));
        return false;
    }
    DEBUG_PRINT(printf("No duplicates of %d in row %d\n", value, row));
    if (duplicate_in_col(b, col, value))
    {
        DEBUG_PRINT(printf("Found duplicate of %d in col %d\n", value, col));
        return false;
    }
    DEBUG_PRINT(printf("No duplicates of %d in col %d\n", value, col));

    if (duplicate_in_box(b, row, col, value))
    {
        DEBUG_PRINT(printf("Found duplicate of %d in box %d\n", value, get_box_idx(b, row, col)));
        return false;
    }
    DEBUG_PRINT(printf("No duplicates of %d in box %d. All looks good.\n", value, get_box_idx(b, row, col)));
    return true;
}
