#pragma once
#include <stdbool.h>
#include <assert.h>

#include "board.h"
bool check_mask(Mask *mask, int value);

bool duplicate_in_row(Board *b, int r, int value);

bool duplicate_in_col(Board *b, int c, int value);

/// @brief checks if there is a duplicate in the box
/// @param b the board to check
/// @param x leftmost cell of box
/// @param y topmost cell of box
/// @param value the value to check for duplicates of
/// @return a boolean indicating if there is a duplicate in the box
bool duplicate_in_box(Board *b, int x, int y, int value);

// this is only called after the sudoku board has been solved to verify the solution
bool validate_board(Board *b);

/// @brief checks if an update resultet in the board being invalid
/// @param b the board
/// @param x the x coordinate of the update
/// @param y the y coordinate of the update
/// @return a boolean indicating if the update was valid
bool validate_update(Board *b, int x, int y, int value);