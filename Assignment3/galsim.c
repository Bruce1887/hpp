#include <stdio.h>
#include <math.h>

#include "logic_stuff.h"
#include "utilities.h"

#include "graphics/graphics.h"

#include "galsim.h"

int N;
char *filename;
int nsteps;
double delta_t;
int graphics_enabled; // 0 or 1

const float circleRadius = 0.0025, circleColor = 0.0F;
const int windowWidth = 800;

Particle *particles;

// print usage
void usage(char *program_name)
{
    printf("Usage: %s <N> <filename> <nsteps> <delta_t> <graphics>\n", program_name);
}

// cleanup allocated memory
void cleanup()
{
    free(particles);
}

int main(int argc, char *argv[])
{
    // check for correct number of arguments
    if (argc != 6)
    {
        usage(argv[0]);
        return 1;
    }

    // set global variables
    N = atoi(argv[1]);
    filename = argv[2];
    nsteps = atoi(argv[3]);
    delta_t = atof(argv[4]);
    graphics_enabled = atoi(argv[5]);

    // read the file
    particles = read_gal_file(filename,N);

    // print the particles
    // for (int i = 0; i < N; i++) {
    //     printf("Particle %d: pos_x = %f, pos_y = %f, mass = %f, vel_x = %f, vel_y = %f, brightness = %f\n", i, particles[i].pos_x, particles[i].pos_y, particles[i].mass, particles[i].vel_x, particles[i].vel_y, particles[i].brightness);
    // }

    if (graphics_enabled) {
        InitializeGraphics("N-Body Simulation", 1000, 1000);
        SetCAxes(0, 1); // Assuming the domain is (0,1)x(0,1)
    }

    // Run simulation
    for (int i = 0; i < nsteps; i++) {
        double *forces_over_mass = calculate_forces_over_mass(particles, N); // Step 1: Compute accelerations
        update_particles(particles, forces_over_mass, N, delta_t); // Step 2: Update positions & velocities
        free(forces_over_mass); // Clean up memory

        // Draw graphics if enabled
        if (graphics_enabled) {
            ClearScreen();
            for (int j = 0; j < N; j++) {
                DrawCircle(particles[j].pos_x, particles[j].pos_y, 1, 1, circleRadius, circleColor);
            }
            Refresh();
            usleep(3000); // Delay for smoother animation
        }
    }

    // write the particles to a file under our_outputs/

    printf("Simulation complete. Press any key to exit...\n");
    getchar();

    write_particles_to_file(particles, N, filename);

    // cleanup allocated memory
    cleanup();

    return 0;
}   