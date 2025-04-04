#include <stdio.h>
#include <math.h>

#include "graphics/graphics.h"

#include <sys/time.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <string.h>

#include <assert.h>
int N;
char *filename;
int nsteps;
double delta_t;
int graphics_enabled; // 0 or 1

const float circleRadius = 0.0025, circleColor = 0.0F;
const int windowWidth = 800;

double *force_buf;

double *P_pos_x = NULL;
double *P_pos_y = NULL;
double *P_mass = NULL;
double *P_vel_x = NULL;
double *P_vel_y = NULL;
double *P_brightness = NULL;

void allocate_particle_buffers(int N)
{
    P_pos_x = (double *)malloc(N * sizeof(double));
    P_pos_y = (double *)malloc(N * sizeof(double));
    P_mass = (double *)malloc(N * sizeof(double));
    P_vel_x = (double *)malloc(N * sizeof(double));
    P_vel_y = (double *)malloc(N * sizeof(double));
    P_brightness = (double *)malloc(N * sizeof(double));
}

void free_particles()
{
    free(P_pos_x);
    free(P_pos_y);
    free(P_mass);
    free(P_vel_x);
    free(P_vel_y);
    free(P_brightness);
}

// print usage
void usage(char *program_name)
{
    printf("Usage: %s <N> <filename> <nsteps> <delta_t> <graphics>\n", program_name);
}

// cleanup allocated memory
void cleanup()
{
    free_particles();
    free(force_buf);
}

// returns an allocated buffer containing the contents of the file as particles
void read_gal_file(char *filename, int N)
{
    assert(P_pos_x);
    assert(P_pos_y);
    assert(P_mass);
    assert(P_vel_x);
    assert(P_vel_y);
    assert(P_brightness);

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

    for (int i = 0; i < N; i++)
    {
        P_pos_x[i] = buffer[i * 6];

        P_pos_y[i] = buffer[i * 6 + 1];
        P_mass[i] = buffer[i * 6 + 2];
        P_vel_x[i] = buffer[i * 6 + 3];
        P_vel_y[i] = buffer[i * 6 + 4];
        P_brightness[i] = buffer[i * 6 + 5];
    }
    free(buffer);
}

void write_particles_to_file(int N)
{
    char *output_file = "result.gal";

    FILE *file = fopen(output_file, "wb");

    if (file == NULL)
    {
        fprintf(stderr, "Error opening file for writing: %s\n", output_file);
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < N; i++)
    {
        double data[6] = {P_pos_x[i], P_pos_y[i], P_mass[i], P_vel_x[i], P_vel_y[i], P_brightness[i]};
        fwrite(data, sizeof(double), 6, file);
    }

    printf("wrote final state to %s\n", output_file);
    fclose(file);
}

static inline void calculate_forces_over_mass(int N, double *buf)
{
    double G_over_N = 100.0 / N; // Given in the assignment
    double epsilon0 = 1e-3;      // Plummer softening

    // reset for each computation so previous forces are not included
    memset(buf, 0, 2 * N * sizeof(double));

    for (int i = 0; i < N; i++)
    {
        double p_pos_x_i = P_pos_x[i];
        double p_pos_y_i = P_pos_y[i];

        double buf_2i = buf[2 * i];
        double buf_2i1 = buf[2 * i + 1];
        double mass_i = P_mass[i];

#pragma GCC ivdep
        for (int j = i + 1; j < N; j++)
        {
            double dx = P_pos_x[j] - p_pos_x_i;
            double dy = P_pos_y[j] - p_pos_y_i;
            double r2 = dx * dx + dy * dy;
            double r = sqrt(r2) + epsilon0;
            double F = G_over_N / (r * r * r);

            double Fx = F * dx;
            double Fy = F * dy;

            // Apply force to particle i
            buf_2i += Fx * P_mass[j]; 
            buf_2i1 += Fy * P_mass[j];

            // Apply equal & opposite force to j
            buf[2 * j] -= Fx * mass_i; 
            buf[2 * j + 1] -= Fy * mass_i;
        }
        buf[2 * i] = buf_2i;
        buf[2 * i + 1] = buf_2i1;
    }
}

// Update positions and velocities using symplectic Euler
static inline void update_particles(double *restrict forces_over_mass, int N, double delta_t)
{
    for (int i = 0; i < N; i++)
    {
        P_vel_x[i] += forces_over_mass[2 * i] * delta_t;
        P_vel_y[i] += forces_over_mass[2 * i + 1] * delta_t;

        P_pos_x[i] += P_vel_x[i] * delta_t;
        P_pos_y[i] += P_vel_y[i] * delta_t;
    }
}

static inline void no_graphics_loop()
{
    for (int i = 0; i < nsteps; i++)
    {
        calculate_forces_over_mass(N, force_buf); // Step 1: Compute accelerations
        update_particles(force_buf, N, delta_t);  // Step 2: Update positions & velocities
    }
}

static inline void graphics_loop()
{
    for (int i = 0; i < nsteps; i++)
    {
        calculate_forces_over_mass(N, force_buf); // Step 1: Compute accelerations
        update_particles(force_buf, N, delta_t);  // Step 2: Update positions & velocities

        // Draw graphics if enabled
        ClearScreen();
        for (int j = 0; j < N; j++)
        {
            DrawCircle(P_pos_x[j], P_pos_y[j], 1, 1, circleRadius, circleColor);
        }
        Refresh();
        usleep(3000); // Delay for smoother animation
    }
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

    allocate_particle_buffers(N);
    printf("graphics_enabled: %d\n", graphics_enabled);
    // read the file
    read_gal_file(filename, N);

    if (graphics_enabled)
    {
        InitializeGraphics("N-Body Simulation", 1000, 1000);
        SetCAxes(0, 1); // Assuming the domain is (0,1)x(0,1)
    }

    force_buf = (double *)malloc(2 * N * sizeof(double));

    // Start timing
    puts("Start timing...");
    struct timeval start, end;
    gettimeofday(&start, NULL);

    // Run simulation
    if (graphics_enabled)
    {
        graphics_loop();
    }
    else
    {
        no_graphics_loop();
    }

    // End timing
    gettimeofday(&end, NULL);
    // Compute elapsed time in seconds
    double elapsed = (end.tv_sec - start.tv_sec) +
                     (end.tv_usec - start.tv_usec) / 1e6;
    printf("Timing finished. Elapsed time: %.6f seconds\n", elapsed);

    // write the particles to a file under our_outputs/
    write_particles_to_file(N);

    // cleanup allocated memory
    cleanup();

    return 0;
}
