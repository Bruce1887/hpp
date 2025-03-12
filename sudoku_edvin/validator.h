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
                // DEBUG_PRINT(printf("Duplicate in row at %d\n", r);)
                // DEBUG_PRINT(printf("apparently %d is equal to %d\n", cell, other);)

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
                // DEBUG_PRINT(printf("Duplicate in col at %d\n", c);)
                // DEBUG_PRINT(printf("apparently %d is equal to %d\n", cell, other);)
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
bool duplicate_in_box(Board *b, int x, int y)
{
    DEBUG_ASSERT(x % b->base == 0);
    DEBUG_ASSERT(y % b->base == 0);

    bool seen[b->side + 1]; // Tracks seen numbers (1 to side)
    for (int i = 0; i <= b->side; i++)
        seen[i] = false;

    for (int r = 0; r < b->base; r++)
    {
        for (int c = 0; c < b->base; c++)
        {
            Cell cell = get_cell(b, x + c, y + r);
            if (IS_EMPTY(cell))
                continue; // Skip empty cells

            if (seen[cell])
                return true; // Duplicate found

            seen[cell] = true; // Mark number as seen
        }
    }
    return false; // No duplicates found
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

    //  DEBUG_PRINT(printf("Found no duplicates in any rows or columns\n");)

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
    // DEBUG_PRINT(printf("Found no duplicates in any boxes\n"));
    return true;
}

/// @brief checks if an update resultet in the board being invalid
/// @param b the board
/// @param x the x coordinate of the update
/// @param y the y coordinate of the update
/// @return a boolean indicating if the update was valid
bool validate_update(Board *b, int x, int y)
{
    bool valid = true;

#pragma omp parallel sections shared(valid)
    {
#pragma omp section
        if (duplicate_in_row(b, y))
            valid = false;
#pragma omp section
        if (duplicate_in_col(b, x))
            valid = false;

#pragma omp section
        {
            int bx = x - x % b->base;
            int by = y - y % b->base;
            if (duplicate_in_box(b, bx, by))
                valid = false;
        }
    }

    return valid;
}


