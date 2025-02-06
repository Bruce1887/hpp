#pragma once

#include <stdio.h>
#include <stdlib.h>

// double const epsilon_0 = 1e-3;


#define OUTPUT_DIR "our_outputs"

typedef struct {
    double pos_x;
    double pos_y;
    double mass;
    double vel_x;
    double vel_y;
    double brightness;
} Particle;


void usage(char *program_name);
void cleanup();
