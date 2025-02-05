#pragma once

#include <stdio.h>
#include <stdlib.h>

// double const G = 6.6740e-11;
#define G 6.6740e-11 // vi kör define istället för const 
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
Particle* read_gal_file(char* filename);