#pragma once

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "solver.h"
#include <string.h>

Board* read_dat_file(char *filename, int N)
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

    int num_int = file_size / sizeof(int8_t);
    int8_t *buffer = (int8_t *)malloc(file_size);
    if (buffer == NULL)
    {
        perror("Error allocating memory");
        fclose(file);
        exit(EXIT_FAILURE);
    }

    size_t read_elements = fread(buffer, sizeof(int8_t), num_int, file);
    if (read_elements != num_int)
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
    b->cells = malloc((num_int-2) * sizeof(int8_t));
    b->cells = memcpy(b->cells, buffer + 2, num_int-2);
    free(buffer);
    return b;
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

    int8_t data[num_cells +2];
    data[0] = b->base;
    data[1] = b->side;
    mempcpy(data + 2, b->cells, b->side * b->side);

    fwrite(data, sizeof(int8_t), num_cells + 2, file);
    
    printf("wrote final state to %s\n", output_file);
    fclose(file);
}