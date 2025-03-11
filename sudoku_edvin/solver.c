/**
 * @file solver.c
 * @author Edvin Bruce (student id edbr8688)
 * @brief Project in HPP Spring 2025
 * @date 2025-03-10
 */

#include <stdio.h>
#include "parser.h"
#include "solver.h"
#include "validator.h"

bool solve_my_board(Board *b)
{
    if (b->num_empty == 0)
        return true;

    int8_t idx = b->empty_chain->idx;
    // try all possible values in that cell
    for (int val = 1; val <= b->side; val++)
    {
        b->cells[idx] = val;
        
        int x;
        int y;
        get_coords(b, idx, &x, &y);

        if (validate_update(b, x, y))
        { 
            EmptyChain *tmp = b->empty_chain;
            b->empty_chain = b->empty_chain->next;
            b->num_empty--;

            if (solve_my_board(b))
                return true;

            b->empty_chain = tmp;
            b->num_empty++;
        }
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
    print_board(b);

    bool solved = solve_my_board(b);
    print_board(b);
    printf("Board is %s\n", solved ? "solved" : "not solved");

    write_board_to_file(b);

    delete_board(b);
    exit(EXIT_SUCCESS);
}