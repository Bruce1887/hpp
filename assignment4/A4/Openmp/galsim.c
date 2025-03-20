#include <stdio.h>
#include <math.h>

#include "../graphics.h"

#include <sys/time.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <string.h>

#include <assert.h>
#include <omp.h>

#include <pthread.h>
#include <stdatomic.h>

int num_threads = -1;
#ifdef PTHREADS
atomic_int pthreads_idx = 0;

pthread_barrier_t barrier; // den här funkar, men får röd markering i VS Code (iaf för mig /edvin)

typedef struct thread_data_struct
{
    int id;
    double *local_force_buf;
    int local_tick;
} thread_data;

pthread_t *threads;
thread_data **all_data;
void setup_threads(int num_threads, int num_particles)
{
    if (num_threads < 1)
    {
        fprintf(stderr, "Invalid number of threads\n");
        exit(1);
    }
    threads = (pthread_t *)malloc(num_threads * sizeof(pthread_t));
    all_data = calloc(num_threads, sizeof(thread_data *));

    for (int i = 0; i < num_threads; i++)
    {
        thread_data *data = (thread_data *)malloc(sizeof(thread_data));
        data->id = i;
        data->local_force_buf = (double *)calloc(2 * num_particles, sizeof(double));
        data->local_tick = 0;
        all_data[i] = data;
    }
    // setup barrier
    pthread_barrier_init(&barrier, NULL, num_threads);
}
void cleanup_Pthreads(int num_threads)
{
    for (int i = 0; i < num_threads; i++)
    {
        free(all_data[i]->local_force_buf);
        free(all_data[i]);
    }
    free(all_data);
    free(threads);
}
#endif

int N;
char *filename;
int nsteps;
double delta_t;
int graphics_enabled; // 0 or 1

const float circleRadius = 0.0025, circleColor = 0.0F;
const int windowWidth = 800;

double *global_force_buf;

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
#ifdef SEQ
    printf("Usage: %s <N> <filename> <nsteps> <delta_t> <graphics>\n", program_name);
#else
    printf("Usage: %s <N> <filename> <nsteps> <delta_t> <graphics> <num_threads>\n", program_name);
#endif
}

// cleanup allocated memory
void cleanup()
{
    free_particles();
    free(global_force_buf);
#ifdef PTHREADS
    cleanup_Pthreads(num_threads);
#endif
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

    // printf("File size: %ld\n", file_size);
    int num_particles_in_file = file_size / (6 * sizeof(double));
    // printf("Num particles in file: %d\n", num_particles_in_file);

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

static inline void calculate_forces_over_mass_OPENMP_REDUCE(int N, double *buf)
{
    double G_over_N = 100.0 / N; // Given in the assignment
    double epsilon0 = 1e-3;      // Plummer softening

    memset(buf, 0, 2 * N * sizeof(double)); // Reset forces between ticks

    #pragma omp parallel for schedule(dynamic,8) reduction(+:buf_private[:N])
    for (int i = 0; i < N; i++)
    {
        double p_pos_x_i = P_pos_x[i];
        double p_pos_y_i = P_pos_y[i];

        double buf_2i = 0.0;
        double buf_2i1 = 0.0;
        double mass_i = P_mass[i];

        for (int j = i + 1; j < N; j++)
        {
            double dx = P_pos_x[j] - p_pos_x_i;
            double dy = P_pos_y[j] - p_pos_y_i;
            double r2 = dx * dx + dy * dy;
            double r = sqrt(r2) + epsilon0;
            double F = G_over_N / (r * r * r);

            double Fx = F * dx;
            double Fy = F * dy;

            // Accumulate force for i
            buf_2i += Fx * P_mass[j];
            buf_2i1 += Fy * P_mass[j];

            // Accumulate opposite force for j (no atomic needed!)
            buf_private[2 * j] -= Fx * mass_i;
            buf_private[2 * j + 1] -= Fy * mass_i;
        }

        buf_private[2 * i] += buf_2i;
        buf_private[2 * i + 1] += buf_2i1;
    }

}


static inline void calculate_forces_over_mass_OPENMP(int N, double *buf)

{
    double G_over_N = 100.0 / N; // Given in the assignment
    double epsilon0 = 1e-3;      // Plummer softening

    memset(buf, 0, 2 * N * sizeof(double)); // Reset forces between ticks

#ifdef OPENMP
#pragma omp parallel
#endif
    {
        // thread local buffer
        double *buf_private = (double *)calloc(2 * N, sizeof(double));

#ifdef OPENMP
#pragma omp for
#endif
        for (int i = 0; i < N; i++)
        {
            double p_pos_x_i = P_pos_x[i];
            double p_pos_y_i = P_pos_y[i];

            double buf_2i = 0.0;
            double buf_2i1 = 0.0;
            double mass_i = P_mass[i];

            for (int j = i + 1; j < N; j++)
            {
                double dx = P_pos_x[j] - p_pos_x_i;
                double dy = P_pos_y[j] - p_pos_y_i;
                double r2 = dx * dx + dy * dy;
                double r = sqrt(r2) + epsilon0;
                double F = G_over_N / (r * r * r);

                double Fx = F * dx;
                double Fy = F * dy;

                // Accumulate force for i
                buf_2i += Fx * P_mass[j];
                buf_2i1 += Fy * P_mass[j];

                // Accumulate opposite force for j (no atomic needed!)
                buf_private[2 * j] -= Fx * mass_i;
                buf_private[2 * j + 1] -= Fy * mass_i;
            }

            buf_private[2 * i] += buf_2i;
            buf_private[2 * i + 1] += buf_2i1;
        }

        // Merge thread-local results into global buffer
#ifdef OPENMP
#pragma omp critical
#endif
        {
            for (int k = 0; k < 2 * N; k++)
            {
                buf[k] += buf_private[k];
            }
        }

        free(buf_private);
    }
}
static inline void update_particles_OPENMP(double *restrict forces_over_mass, int N, double delta_t)
{
#ifdef OPENMP
#pragma omp parallel for
#endif
    for (int i = 0; i < N; i++)
    {
        P_vel_x[i] += forces_over_mass[2 * i] * delta_t;
        P_vel_y[i] += forces_over_mass[2 * i + 1] * delta_t;

        P_pos_x[i] += P_vel_x[i] * delta_t;
        P_pos_y[i] += P_vel_y[i] * delta_t;
    }
}

#ifdef PTHREADS
static inline void calculate_force_over_mass_for_one(thread_data *t_data, int i, double epsilon0, double G_over_N);
void *pthread_func(void *arg)
{
    // ### calculate forces over mass ###
    thread_data *t_data = (thread_data *)arg;

    double G_over_N = 100.0 / N; // Given in the assignment
    double epsilon0 = 1e-3;      // Plummer softening

    // this while loop is for all the ticks. We leave this loop when we have processed all ticks
    while (1)
    {
        if (t_data->local_tick == nsteps)
        {
            break;
        }
        assert(t_data->local_tick < nsteps);
        t_data->local_tick++;

        // Reset forces between ticks
        memset(t_data->local_force_buf, 0, 2 * N * sizeof(double)); // all threads reset their local buffers
        if (t_data->id == 0)
            memset(global_force_buf, 0, 2 * N * sizeof(double)); // thread 0 resets the global buffer

        // do work for this tick. we leave this while loop when we have processed all particles
        while (1)
        {
            int particle_idx = atomic_fetch_add(&pthreads_idx, 1);
            if (particle_idx >= N)
                break;

            calculate_force_over_mass_for_one(t_data, particle_idx, epsilon0, G_over_N);
        }

        pthread_barrier_wait(&barrier);

        // KRITISK SEKTION, vi säger att tråd 0 kör sista biten här bara
        if (t_data->id == 0)
        {
            memset(global_force_buf, 0, 2 * N * sizeof(double)); // Reset global forces
            // combine the local buffers from all the threads into common buffer
            for (int j = 0; j < 2 * N; j++)
            {
                for (int x = 0; x < num_threads; x++)
                {
                    global_force_buf[j] += all_data[x]->local_force_buf[j];
                }
            }
            // ### Update particles ###
            for (int idx = 0; idx < N; idx++)
            {
                P_vel_x[idx] += global_force_buf[2 * idx] * delta_t;
                P_vel_y[idx] += global_force_buf[2 * idx + 1] * delta_t;

                P_pos_x[idx] += P_vel_x[idx] * delta_t;
                P_pos_y[idx] += P_vel_y[idx] * delta_t;
            }
            // reset pthreads_idx 
            pthreads_idx = 0;
        }
        pthread_barrier_wait(&barrier);
    }
    return NULL;
}

static inline void calculate_force_over_mass_for_one(thread_data *t_data, int particle_idx, double epsilon0, double G_over_N)
{
    double p_pos_x_i = P_pos_x[particle_idx];
    double p_pos_y_i = P_pos_y[particle_idx];

    double buf_2i = 0.0;
    double buf_2i1 = 0.0;
    double mass_i = P_mass[particle_idx];

    int idx_helper = particle_idx + 1; // check for all after particle_idx
    while (idx_helper < N)
    {
        double dx = P_pos_x[idx_helper] - p_pos_x_i;
        double dy = P_pos_y[idx_helper] - p_pos_y_i;
        double r2 = dx * dx + dy * dy;
        double r = sqrt(r2) + epsilon0;
        double F = G_over_N / (r * r * r);

        double Fx = F * dx;
        double Fy = F * dy;

        // Apply force to particle
        buf_2i += Fx * P_mass[idx_helper];
        buf_2i1 += Fy * P_mass[idx_helper];

        // Apply equal & opposite force to other particle
        t_data->local_force_buf[2 * idx_helper] -= Fx * mass_i;
        t_data->local_force_buf[2 * idx_helper + 1] -= Fy * mass_i;
        idx_helper++;
    }

    t_data->local_force_buf[2 * particle_idx] += buf_2i;
    t_data->local_force_buf[2 * particle_idx + 1] += buf_2i1;
}
#endif

static inline void no_graphics_loop()
{

#ifdef PTHREADS
    for (int i = 0; i < num_threads; i++)
    {
        pthread_create(&threads[i], NULL, pthread_func, all_data[i]);
    }

    for (int i = 0; i < num_threads; i++)
    {
        pthread_join(threads[i], NULL);
    }

    return;
#else
    for (int i = 0; i < nsteps; i++)
    {
        calculate_forces_over_mass_OPENMP(N, global_force_buf);
        update_particles_OPENMP(global_force_buf, N, delta_t);
    }
#endif
}

static inline void graphics_loop()
{
    for (int i = 0; i < nsteps; i++)
    {
#ifndef PTHREADS
        calculate_forces_over_mass_OPENMP(N, global_force_buf);
        update_particles_OPENMP(global_force_buf, N, delta_t);
#else
        fprintf(stderr, "Graphics not supported with PTHREADS\nExiting...\n");
        exit(1);
#endif

        // Draw graphics
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
#ifdef SEQ
    if (argc != 6)
#else
    if (argc != 7)
#endif
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
#ifdef SEQ
    num_threads = 1;
#else
    num_threads = atoi(argv[6]);
#endif

#ifdef OPENMP
    // #pragma omp parallel num_threads(num_threads)
    puts("### Using OpenMP ###");
    omp_set_num_threads(num_threads);
    printf("num_threads: %d\n", num_threads);
#endif

#ifdef PTHREADS
    puts("### Using Pthreads ###");
    printf("num_threads: %d\n", num_threads);
    setup_threads(num_threads, N);
#endif

#if !defined(OPENMP) && !defined(PTHREADS)
    puts("### Using Sequential implementation ###");
#endif

    allocate_particle_buffers(N);
    // read the file
    read_gal_file(filename, N);

    if (graphics_enabled)
    {
        InitializeGraphics("N-Body Simulation", 1000, 1000);
        SetCAxes(0, 1); // Assuming the domain is (0,1)x(0,1)
    }

    global_force_buf = (double *)malloc(2 * N * sizeof(double));

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
