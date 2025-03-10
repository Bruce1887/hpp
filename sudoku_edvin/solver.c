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

void usage(char *program_name) {
    printf("Usage: %s <filename> \n", program_name);
    exit(EXIT_FAILURE);
}

void print_board(Board *b) {
    for (int i = 0; i < b->side; i++) {
        for (int j = 0; j < b->side; j++) {
            printf("%d ", b->cells[i * b->side + j]);
        }
        printf("\n");
    }
}

void solve_my_board(Board *b) {

}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        usage(argv[0]);
    }

    Board* b = read_dat_file(argv[1], 0);
    printf("base: %d\n", b->base);
    printf("side: %d\n", b->side);

    write_board_to_file(b);
    // print_board(b);

    bool ok = validate_board(b);
    printf("Board is %s\n", ok ? "valid" : "invalid");

    delete_board(b);
    exit(EXIT_SUCCESS);
}