#pragma once

#include <stdio.h>
#include "galsim.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>

// skriva/läsa till fil

// returns an allocated buffer containing the contents of the file as particles
Particle *read_gal_file(char *filename, int N)
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

    Particle *particles = (Particle *)malloc(N * sizeof(Particle));

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

void write_particles_to_file(Particle *particles, int N, char *filename)
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
    printf("wrote final state to %s\n", output_filepath);
    fclose(file);
}