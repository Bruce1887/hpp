#pragma once

#include <stdio.h>
#include <stdlib.h>

#define OUTPUT_DIR "our_outputs"

double *P_pos_x = NULL;
double *P_pos_y = NULL;
double *P_mass = NULL;
double *P_vel_x = NULL;
double *P_vel_y = NULL;
double *P_brightness = NULL;


void usage(char *program_name);
void cleanup();
