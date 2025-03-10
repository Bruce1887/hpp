#pragma once

#include "solver.h"
#include <stdbool.h>
#include <assert.h>

bool duplicate_in_row(Board *b, int r)
{
    for (int c = 0; c < b->side; c++)
    {
        Cell cell = get_cell(b, c, r);
        if (IS_EMPTY(cell))
            continue;

        for (int c2 = c + 1; c2 < b->side; c2++)
        {
            Cell other = get_cell(b, c2, r);
            if (cell == other)
            {
#ifdef DEBUG
                printf("Duplicate in row at %d\n", r);
                printf("apparently %d is equal to %d\n", cell, other);
#endif
                return true;
            }
        }
    }
    return false;
}

bool duplicate_in_col(Board *b, int c)
{
    for (int r = 0; r < b->side; r++)
    {
        Cell cell = get_cell(b, c, r);
        if (IS_EMPTY(cell))
            continue;

        for (int r2 = r + 1; r2 < b->side; r2++)
        {
            Cell other = get_cell(b, c, r2);
            if (cell == other)
            {
#ifdef DEBUG
                printf("Duplicate in col at %d\n", c);
                printf("apparently %d is equal to %d\n", cell, other);
#endif
                return true;
            }
        }
    }
    return false;
}

/// @brief checks if there is a duplicate in the box
/// @param b the board to check
/// @param x leftmost cell of box
/// @param y topmost cell of box
/// @return a boolean indicating if there is a duplicate in the box
bool duplicate_in_box(Board *b, int c, int r)
{
#ifdef DEBUG
    assert(c % b->base == 0);
    assert(r % b->base == 0);
#endif
    for (int c1 = 0; c1 < b->base; c1++)
    {
        for (int r1 = 0; r1 < b->base; r1++)
        {
            Cell cell = get_cell(b, c + c1, r + r1);
            if (IS_EMPTY(cell))
                continue;

            for (int c2 = c + c1 + 1; c2 < b->base; c2++)
            {
                for (int r2 = r + r1 + 1; r2 < b->base; r2++)
                {
                    Cell other = get_cell(b, c2, r2);
                    if (cell == other)
                    {
#ifdef DEBUG
                        printf("Duplicate in box at (%d, %d)\n", c1, c2);
                        printf("apparently %d is equal to %d\n", cell, other);
#endif
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

bool validate_board(Board *b)
{
    for (int i = 0; i < b->side; i++)
    {
        if (duplicate_in_row(b, i))
        {
            return false;
        }
        if (duplicate_in_col(b, i))
        {
            return false;
        }
    }
#ifdef DEBUG
    printf("Found no duplicates in any rows or columns\n");
#endif
    for (int x = 0; x < b->side; x += b->base)
    {
        for (int y = 0; y < b->side; y += b->base)
        {
            if (duplicate_in_box(b, x, y))
            {
                return false;
            }
        }
    }
#ifdef DEBUG
    printf("Found no duplicates in any boxes\n");
#endif
    return true;
}