#pragma once

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "solver.h"
#include <string.h>
#include <assert.h>

void initialise_empty_mask(Board *b);
void initialise_cell_masks(Board *b);

Board *read_dat_file(char *filename, int N)
{
    FILE *file = fopen(filename, "rb");
    if (file == NULL)
    {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    if (file_size % sizeof(int8_t) != 0)
    {
        fprintf(stderr, "File size is not a multiple of sizeof(int8_t)\n");
        fclose(file);
        exit(EXIT_FAILURE);
    }

    int8_t *buffer = (int8_t *)malloc(file_size);
    if (buffer == NULL)
    {
        perror("Error allocating memory");
        fclose(file);
        exit(EXIT_FAILURE);
    }

    size_t read_elements = fread(buffer, sizeof(int8_t), file_size, file);
    if (read_elements != file_size)
    {
        perror("Error reading file");
        free(buffer);
        fclose(file);
        exit(EXIT_FAILURE);
    }
    fclose(file);

    Board *b = (Board *)malloc(sizeof(Board));
    b->base = buffer[0];
    b->side = buffer[1];
    assert(file_size == b->side * b->side + 2);
    assert(b->side % b->base == 0);

    int8_t *cells = buffer + 2;
    b->cells = malloc((file_size - 2) * sizeof(Cell));
    memcpy(b->cells, cells, file_size - 2);
    free(buffer);

    b->num_empty = 0;

    b->mask_size = (b->side * b->side + 8 * sizeof(Mask) - 1) / (8 * sizeof(Mask));
    printf("mask size: %d\n", b->mask_size);
    initialise_empty_mask(b);
    initialise_cell_masks(b);

    printf("file size: %ld\n", file_size);

    return b;
}

void initialise_empty_mask(Board *b)
{
    // initialise the empty mask
    b->empty_mask = (Mask *)calloc(b->mask_size, sizeof(Mask));
    for (int i = 0; i < b->mask_size; i++)
    {
        // initialise all bits to 1
        b->empty_mask[i] = (Mask)~OCCUPIED;
    }
    for (int i = 0; i < b->side * b->side; i++)
    {
        if (IS_EMPTY(b->cells[i]))
        {
            b->num_empty++;
            b->empty_mask[i / 64] &= ~((uint64_t)1 << (i % 64)); // Ensure empty cells are 0
        }
    }
}

// initilises the masks for the rows, columns and boxes
void initialise_cell_masks(Board *b)
{
    assert(b->side % b->base == 0);
    b->num_boxes = b->side / b->base;

    b->r_mask = (Mask **)malloc(b->side * sizeof(Mask *));
    b->c_mask = (Mask **)malloc(b->side * sizeof(Mask *));
    b->b_mask = (Mask **)malloc(b->num_boxes * sizeof(Mask *));

    for (int i = 0; i < b->side; i++)
    {
        b->r_mask[i] = (Mask *)calloc(b->mask_size, sizeof(Mask));
        b->c_mask[i] = (Mask *)calloc(b->mask_size, sizeof(Mask));
        for (int j = 0; j < b->mask_size; j++)
        {
            // initialise all bits to 1
            b->r_mask[i][j] = (Mask)~OCCUPIED;
            b->c_mask[i][j] = (Mask)~OCCUPIED;
        }
    }
    for (int i = 0; i < b->num_boxes; i++)
    {
        b->b_mask[i] = (Mask *)calloc(b->mask_size, sizeof(Mask));
        for (int j = 0; j < b->mask_size; j++)
        {
            // initialise all bits to 1
            b->b_mask[i][j] = (Mask)~OCCUPIED;
        }
    }
}

void write_board_to_file(Board *b)
{
    char *output_file = "output.dat";

    FILE *file = fopen(output_file, "wb");

    if (file == NULL)
    {
        fprintf(stderr, "Error opening file for writing: %s\n", output_file);
        exit(EXIT_FAILURE);
    }

    int num_cells = b->side * b->side;

    int8_t data[num_cells + 2];
    data[0] = b->base;
    data[1] = b->side;
    mempcpy(data + 2, b->cells, b->side * b->side);

    fwrite(data, sizeof(int8_t), num_cells + 2, file);

    printf("wrote final state to %s\n", output_file);
    fclose(file);
}