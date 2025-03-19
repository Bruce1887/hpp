#pragma once

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "solver.h"
#include <string.h>
#include <assert.h>

void initialise_empty_mask(Board *b);
void initialise_cell_masks(Board *b);

Board *read_dat_file(char *filename, int N);

void initialise_empty_mask(Board *b);

// initilises the masks for the rows, columns and boxes
void initialise_cell_masks(Board *b);

void write_board_to_file(Board *b);