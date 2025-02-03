#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include <sys/stat.h>
#include <sys/types.h>

#include "galsim.h"

int N;
char *filename;
int nsteps;
double delta_t;
int graphics_enabled; // 0 or 1

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

// returns an allocated buffer containing the contents of the file as particles
Particle *read_gal_file(char *filename)
{
    FILE *file = fopen(filename, "rb");
    if (file == NULL)
    {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    printf("File size: %ld\n", file_size);
    int num_particles_in_file = file_size / (6 * sizeof(double));
    printf("Num particles in file: %d\n", num_particles_in_file);

    if (file_size % sizeof(double) != 0)
    {
        fprintf(stderr, "File size is not a multiple of sizeof(double)\n");
        fclose(file);
        exit(EXIT_FAILURE);
    }

    if (num_particles_in_file < N)
    {
        fprintf(stderr, "File size is smaller than N\n");
        fclose(file);
        exit(EXIT_FAILURE);
    }

    int num_doubles = file_size / sizeof(double);
    double *buffer = (double *)malloc(file_size);
    if (buffer == NULL)
    {
        perror("Error allocating memory");
        fclose(file);
        exit(EXIT_FAILURE);
    }

    size_t read_elements = fread(buffer, sizeof(double), num_doubles, file);
    if (read_elements != num_doubles)
    {
        perror("Error reading file");
        free(buffer);
        fclose(file);
        exit(EXIT_FAILURE);
    }

    fclose(file);

    particles = (Particle *)malloc(N * sizeof(Particle));

    for (int i = 0; i < N; i++)
    {
        particles[i].pos_x = buffer[i * 6];
        particles[i].pos_y = buffer[i * 6 + 1];
        particles[i].mass = buffer[i * 6 + 2];
        particles[i].vel_x = buffer[i * 6 + 3];
        particles[i].vel_y = buffer[i * 6 + 4];
        particles[i].brightness = buffer[i * 6 + 5];
    }

    free(buffer);
    return particles;
}

void write_particles_to_file()
{
    mkdir(OUTPUT_DIR, 0777); // Creates directory if it doesn't exist


    char *basename = strrchr(filename, '/');
    
    // If a '/' is found, the filename starts after it; otherwise, the whole string is the filename
    if (basename != NULL) {
        basename++;  // Move past the '/' to get the actual filename
    } else {
        basename = filename;  // If no '/', the entire string is the filename
    }
    
    char output_filepath[100];
    sprintf(output_filepath, "%s/output_%s", OUTPUT_DIR, basename);
    FILE *file = fopen(output_filepath, "wb");

    if (file == NULL)
    {
        fprintf(stderr, "Error opening file for writing: %s\n", output_filepath);
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < N; i++)
    {
        fprintf(file, "%f %f %f %f %f %f\n", particles[i].pos_x, particles[i].pos_y, particles[i].mass,
                particles[i].vel_x, particles[i].vel_y, particles[i].brightness);
    }

    fclose(file);
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
    particles = read_gal_file(filename);

    // print the particles
    // for (int i = 0; i < N; i++) {
    //     printf("Particle %d: pos_x = %f, pos_y = %f, mass = %f, vel_x = %f, vel_y = %f, brightness = %f\n", i, particles[i].pos_x, particles[i].pos_y, particles[i].mass, particles[i].vel_x, particles[i].vel_y, particles[i].brightness);
    // }

    for (int i = 0; i < nsteps; i++)
    {
        // update the particles
        for (int j = 0; j < N; j++)
        {
            double Fx = 0;
            double Fy = 0;
            for (int k = 0; k < N; k++)
            {
                if (j != k)
                {
                    double dx = particles[k].pos_x - particles[j].pos_x;
                    double dy = particles[k].pos_y - particles[j].pos_y;
                    double epsilon = 1e-10;
                    double r = sqrt(dx * dx + dy * dy) + epsilon;
                    double F = G * particles[j].mass * particles[k].mass / (r * r);
                    Fx += F * dx / r;
                    Fy += F * dy / r;
                }
            }
            double ax = Fx / particles[j].mass;
            double ay = Fy / particles[j].mass;
            particles[j].vel_x += ax * delta_t;
            particles[j].vel_y += ay * delta_t;
            particles[j].pos_x += particles[j].vel_x * delta_t;
            particles[j].pos_y += particles[j].vel_y * delta_t;
        }
    }

    // write the particles to a file under our_outputs/
    write_particles_to_file();

    // cleanup allocated memory
    cleanup();

    return 0;
}