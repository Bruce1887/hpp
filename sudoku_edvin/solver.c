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

    DEBUG_ASSERT(b->empty_chain != NULL);
    int idx = b->empty_chain->idx;

    #ifdef DEBUG
    EmptyChain *current = b->empty_chain;
    while (current != NULL)
    {
        DEBUG_ASSERT(b->cells[current->idx] == 0);
        current = current->next;
    }
    DEBUG_PRINT(puts("emptychain looks good\n"));
    #endif
    DEBUG_ASSERT(idx < b->side * b->side);
    DEBUG_ASSERT(b->cells[idx] == 0);
    DEBUG_PRINT(printf("b->num_empty: %d\n", b->num_empty));
    
    // try all possible values in that cell
    for (int val = 1; val <= b->side; val++)
    {
        b->cells[idx] = val;
        int x;
        int y;
        get_coords(b, idx, &x, &y);
        // DEBUG_PRINT(printf("testing val %d at (%d,%d)\n", val, x, y));

        if (validate_update(b, x, y))
        {
            EmptyChain *tmp = b->empty_chain;
            b->empty_chain = b->empty_chain->next;
            b->num_empty--;

            if (solve_my_board(b))
            {
                return true;
            }

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

    printf("board first empty idx: %d\n", b->empty_chain->idx);
    printf("board num empty: %d\n", b->num_empty);

    bool solution_found = solve_my_board(b);
    print_board(b);

    printf("Board is %s\n", solution_found ? "solved" : "not solved");

    if (solution_found)
        assert(validate_board(b));

    write_board_to_file(b);

    delete_board(b);
    exit(EXIT_SUCCESS);
}