#pragma once

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "solver.h"
#include <string.h>
#include <assert.h>

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
    b->num_empty = 0;
    b->empty_chain = NULL;

    printf("file size: %ld\n", file_size);

    for (int i = 0; i < file_size - 2; i++)
        if (IS_EMPTY(buffer[i + 2]))
        {
            if (b->empty_chain == NULL)
            {
                b->empty_chain = (EmptyChain *)malloc(sizeof(EmptyChain));
                b->empty_chain->idx = i;
                b->empty_chain->next = NULL;
            }
            else
            {
                EmptyChain *new_chain = (EmptyChain *)malloc(sizeof(EmptyChain));
                new_chain->idx = i;
                new_chain->next = b->empty_chain;
                b->empty_chain = new_chain;
            }
            b->num_empty++;
        }

    // keep track of initial empty cells, they are not freed when backtracking but when deleting the board
    b->num_initial_empty = b->num_empty;
    b->initial_empty = malloc(b->num_empty * sizeof(EmptyChain));
    EmptyChain *ec = b->empty_chain;
    for (int i = 0; i < b->num_empty; i++)
    {
        assert(ec != NULL); // we should have exactly num_empty elements
        b->initial_empty[i] = ec;
        ec = ec->next;
    }

    b->cells = malloc((file_size - 2) * sizeof(int8_t));
    b->cells = memcpy(b->cells, buffer + 2, file_size - 2);
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

    int8_t data[num_cells + 2];
    data[0] = b->base;
    data[1] = b->side;
    mempcpy(data + 2, b->cells, b->side * b->side);

    fwrite(data, sizeof(int8_t), num_cells + 2, file);

    printf("wrote final state to %s\n", output_file);
    fclose(file);
}