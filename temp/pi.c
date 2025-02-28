#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

const int intervals = 500000000;
double dx = 1.0 / intervals;

int *starts;
int *ends;
double *sums;

void *thread_function(void *arg)
{
    int thread_id = *(int*)arg;

    int start = starts[thread_id];
    int end = ends[thread_id];
    printf("id: %d, start: %d, end: %d\n",thread_id, start, end);

    double local_sum = 0;

    for (int i = start; i <= end; i++)
    {
        double x = dx * (i - 0.5);

        local_sum += dx * 4.0 / (1.0 + x * x);
    }

    printf("local_sum: %f\n", local_sum);

    free(arg);
    sums[thread_id] = local_sum;
}

int main(int argc, char *argv[])
{
    int i;
    int num_threads = 4;

    int chunk_size = intervals / num_threads;
    int remainder = intervals % num_threads;

    starts = malloc(sizeof(int) * num_threads);
    ends = malloc(sizeof(int) * num_threads);
    sums = malloc(sizeof(double) * num_threads);

    pthread_t threads[num_threads];

    for (i = 0; i < num_threads; i++)
    {
        int start = i * chunk_size;
        int end = (i + 1) * chunk_size - 1;
        if (i == num_threads - 1)
        {
            end += remainder;
        }
        int* slice = malloc(sizeof(int) * 2);
        starts[i] = start;
        ends[i] = end;

        int *arg = malloc(sizeof(int));
        *arg = i;

        printf("i: %d, start: %d, end: %d\n", i, start, end);
        pthread_create(&threads[i], NULL, thread_function, arg);
    }

    double sum;

    for (i = 0; i < num_threads; i++)
    {
        pthread_join(threads[i], NULL); 
        printf("sums[%d]: %f\n", i, sums[i]);
        sum += sums[i];
    }

    free(starts);
    free(ends);
    free(sums);

    printf("PI is approx. %.16f\n", sum);
}