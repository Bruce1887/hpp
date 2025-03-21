/**
 * @file solver.c
 * @author Edvin Bruce (student id edbr8688)
 * @brief Project in HPP Spring 2025
 * @date 2025-03-21
 */

#include <stdio.h>
#include <time.h>
#include <stdatomic.h>
#include <omp.h>
#include <assert.h>
#include <unistd.h>  // For getpid()
#include <pthread.h>   // For pthread_self()

#include "parser.h"
#include "validator.h"
#include "board.h"

Board *final_board = NULL;
atomic_bool solution_found = false;
double solution_time;
const int SUDOKU_OMP_DEPTH = 48;

void usage(char *program_name)
{
    printf("Usage: %s <filename> <num_threads> \n", program_name);
    exit(EXIT_FAILURE);
}

bool solve_my_board_SEQ(Board *b)
{
    if (b->num_empty == 0)
        return true;

    int idx = get_first_empty(b);
    DEBUG_ASSERT(idx != -1);
    DEBUG_ASSERT(idx < b->side * b->side);
    DEBUG_ASSERT(b->cells[idx] == 0);

    // try all possible values in that cell
    for (int val = 1; val <= b->side; val++)
    {
        b->cells[idx] = val;
        update_mask(b->empty_mask, idx + 1, OCCUPIED); // this "idx +1 " is wonky, but it works
        b->num_empty--;

        int row;
        int col;
        get_coords(b, idx, &row, &col);

        if (validate_update(b, row, col, val))
        {
            update_cell_masks(b, row, col, val, OCCUPIED);
            if (solve_my_board_SEQ(b))
            {
                return true;
            }
            else
            {
                // undo the update
                update_cell_masks(b, row, col, val, VACANT);
            }
        }
        update_mask(b->empty_mask, idx + 1, VACANT); // this "idx +1 " is wonky, but it works
        b->num_empty++;
        b->cells[idx] = VACANT;
    }

    b->cells[idx] = 0;
    return false;
}

#ifdef _OPENMP
void solve_my_board_OPENMP(Board *b);
void solve_for_one(Board *b, int val)
{
    if (solution_found)
    {
        delete_board(b);
        return;
    }
    int idx = get_first_empty(b);
    b->cells[idx] = val;
    update_mask(b->empty_mask, idx + 1, OCCUPIED); // this "idx +1 " is wonky, but it works
    b->num_empty--;

    int row;
    int col;
    get_coords(b, idx, &row, &col);

    if (validate_update(b, row, col, val))
    {
        update_cell_masks(b, row, col, val, OCCUPIED);

        solve_my_board_OPENMP(b);
    }

    // we dont undo the update if it is invalid, we just delete the local board instead
    delete_board(b);
}

void solve_my_board_OPENMP(Board *b)
{
    // printf("omp_level(): %d, pthread_self: %ld\n",omp_get_level(), pthread_self());
    bool expected = true;
    bool new = false;
    if (atomic_compare_exchange_strong(&solution_found, &expected, new))
    {
        return;
    }
    if (b->num_empty == 0)
    {
        bool expected = false;
        bool new = true;
        if (atomic_compare_exchange_strong(&solution_found, &expected, new))
        {
            DEBUG_ASSERT(final_board == NULL);
            final_board = deep_copy_board(b);
            DEBUG_ASSERT(validate_board(b));
            DEBUG_ASSERT(solution_found);
        }
        return;
    }

    if (omp_get_level() > SUDOKU_OMP_DEPTH)
    {
        if (solve_my_board_SEQ(b))
        {
            DEBUG_ASSERT(final_board == NULL);
            final_board = deep_copy_board(b);
            DEBUG_ASSERT(validate_board(b));
            DEBUG_ASSERT(solution_found);
        }
        return;
    }

#pragma omp parallel
    for (int val = 1; val <= b->side; val++)
    {
#pragma omp single nowait
        {
#pragma omp task
            {
                if (solution_found == false)
                {
                    Board *new_board = deep_copy_board(b);
                    solve_for_one(new_board, val);
                }
            }
        }
    }
}

void solve_omp_init(){
    #pragma omp parallel
    {
        #pragma omp single
        {
            Board *b = read_dat_file("sudoku.dat", 0);
            solve_my_board_OPENMP(b);
        }
    }
}
#endif

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        usage(argv[0]);
    }

#ifdef _OPENMP
    printf("OpenMP version: %d\n", _OPENMP);
    omp_set_num_threads(atoi(argv[2]));
    printf("Number of threads: %d\n", atoi(argv[2]));
    printf("max recursive depth: %d\n", SUDOKU_OMP_DEPTH);
    omp_set_nested(1);
#endif
#ifdef SEQ
    puts("Running sequentially");
#endif

    Board *b = read_dat_file(argv[1], 0);

    printf("\nStarting timer...\n");

#ifdef SEQ
    clock_t start_time = clock();
    solve_my_board_SEQ(b);
    clock_t end_time = clock();
    double time_spent = (double)(end_time - start_time) / CLOCKS_PER_SEC;
    printf("Time taken to solve the board: %f seconds\n", time_spent);
#else
    double start_time, end_time;
    start_time = omp_get_wtime();
    solve_my_board_OPENMP(b);
    end_time = omp_get_wtime();
    printf("Time taken to solve the board: %f seconds\n", end_time - start_time);
    // printf("time to find solution: %f\n", solution_time - start_time);
#endif

#ifdef SEQ
    assert(validate_board(b));
#else
    if (final_board)
    {
        assert(validate_board(final_board));
        printf("Board validated succesfully\n");
        write_board_to_file(final_board);
        delete_board(final_board);
    }
    else
    {
        printf("No solution found\n");
    }
#endif

    delete_board(b);

    exit(EXIT_SUCCESS);
}