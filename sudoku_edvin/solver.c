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
#include <time.h>

bool solve_my_board(Board *b)
{
    if (b->num_empty == 0)
        return true;

    int idx = get_first_empty(b);
    DEBUG_ASSERT(idx != -1);
    // print_mask(b);
    DEBUG_ASSERT(idx < b->side * b->side);
    DEBUG_ASSERT(b->cells[idx] == 0);
    DEBUG_PRINT(printf("b->num_empty: %d\n", b->num_empty));
    
    // try all possible values in that cell
    for (int val = 1; val <= b->side; val++)
    {
        b->cells[idx] = val;
        update_mask(b->empty_mask, idx, OCCUPIED);
        b->num_empty--;

        int x;
        int y;
        get_coords(b, idx, &x, &y);

        if (validate_update(b, x, y))
        {

            if (solve_my_board(b))
            {
                return true;
            }
        }
            update_mask(b->empty_mask, idx, VACANT);
            b->num_empty++;
            b->cells[idx] = VACANT;
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

    printf("board num empty: %d\n", b->num_empty);

    clock_t start_time = clock();
    bool solution_found = solve_my_board(b);
    clock_t end_time = clock();
    double time_spent = (double)(end_time - start_time) / CLOCKS_PER_SEC;
    
    printf("Time taken to solve the board: %f seconds\n", time_spent);

    printf("Board is %s\n", solution_found ? "solved" : "not solved");
    
    print_board(b);
    if (solution_found)
        assert(validate_board(b));
    

    write_board_to_file(b);

    delete_board(b);
    exit(EXIT_SUCCESS);
}