#include <stdio.h>
#include <stdlib.h>

void usage(char *prog_name) {
    printf("Usage: %s <N>, where N is a positive integer\n", prog_name);
}

int main(char argc, char *argv[]) {
    
    if (argc != 2) {
        usage(argv[0]);
        return 1;
    }

    int rows = atoi(argv[1]);
    
    if (rows <= 0) {
        usage(argv[0]);
        return 1;
    }

    for (int n = 0; n < rows; n++) {
        for (int k = 0; k < n+1; k++) {
            float x = 1;
            for (float i = 1; i <= k; i++) {
                x *= (n + 1 - i)/i;
            }
            printf("%d  ", (int)x);
        }
        printf("\n");
    }

    return 0;
}