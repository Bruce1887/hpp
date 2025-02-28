#include <stdio.h>

#include <pthread.h>

int DoItNow = 0;

pthread_mutex_t m;

pthread_cond_t c = PTHREAD_COND_INITIALIZER;

void *thread_func(void *arg)
{

    printf("This is thread_func() starting, now entering loop to wait until DoWorkNow is set...\n");

    pthread_mutex_lock(&m);
    pthread_cond_wait(&c, &m);
    pthread_mutex_unlock(&m);

    printf("This is thread_func() after the loop.\n");

    return NULL;
}

int main()
{

    printf("This is the main() function starting.\n");

    pthread_mutex_init(&m, NULL);

    /* Start thread. */

    pthread_t thread;

    pthread_cond_init(&c, NULL);

    printf("the main() function now calling pthread_create().\n");

    pthread_create(&thread, NULL, thread_func, NULL);

    printf("This is the main() function after pthread_create()\n");

    /* Here we let the main thread do some work. */

    long int k;

    double x = 1;

    for (k = 0; k < 2000000000; k++)

        x *= 1.00000000001;

    printf("main thread did some work, x = %f\n", x);

    pthread_mutex_lock(&m);

    pthread_cond_signal(&c);

    pthread_mutex_unlock(&m);

    /* Wait for thread to finish. */

    printf("the main() function now calling pthread_join().\n");

    pthread_join(thread, NULL);

    printf("This is the main() function after calling pthread_join().\n");

    pthread_mutex_destroy(&m);

    return 0;
}