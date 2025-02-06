#pragma once

#include "galsim.h"
#include <stdio.h>

// Båda formlerna för det här finns i assignmentinstruktionerna "https://uppsala.instructure.com/courses/95091/files/7929374?wrap=1"

double *calculate_forces_over_mass(Particle *particles, int N)
{
    double *forces_over_mass = (double *)calloc(2 * N, sizeof(double)); // Store ax, ay for each particle
    if (forces_over_mass == NULL)
    {
        perror("Memory allocation failed");
        exit(EXIT_FAILURE);
    }

    double G_over_N = 100.0 / N; // Given in the assignment
    double epsilon0 = 1e-3;      // Plummer softening

    for (int i = 0; i < N; i++)
    {
        double Fx = 0, Fy = 0;
        for (int j = 0; j < N; j++)
        {
            if (i != j)
            {
                double dx = particles[j].pos_x - particles[i].pos_x;
                double dy = particles[j].pos_y - particles[i].pos_y;
                double r2 = dx * dx + dy * dy;
                double r = sqrt(r2) + epsilon0;

                double F = G_over_N * particles[j].mass / (r * r * r);
                Fx += F * dx;
                Fy += F * dy;
            }
        }
        forces_over_mass[2 * i] = Fx;   // ax = F/m, already divided by mass in formula
        forces_over_mass[2 * i + 1] = Fy;
    }

    return forces_over_mass;
}

// Update positions and velocities using symplectic Euler
void update_particles(Particle *particles, double *forces_over_mass, int N, double delta_t)
{
    for (int i = 0; i < N; i++)
    {
        particles[i].vel_x += forces_over_mass[2 * i] * delta_t;
        particles[i].vel_y += forces_over_mass[2 * i + 1] * delta_t;

        particles[i].pos_x += particles[i].vel_x * delta_t;
        particles[i].pos_y += particles[i].vel_y * delta_t;
    }
}