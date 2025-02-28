#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

pthread_mutex_t m_thread_count;
int thread_count;

void *thread_fn(void *arg)
{
    int *args = (int *)arg;
    int *list_to_sort = args[0];
    int N = args[1];
    merge_sort(list_to_sort, N);
    free(args);

    pthread_mutex_lock(&m_thread_count);
    thread_count -= 2;
    pthread_mutex_unlock(&m_thread_count);
    return NULL;
}

void merge_sort(int *list_to_sort, int N)
{

    if (N == 1)
    {
        // Only one element, no sorting needed. Just return directly in this case.
        return;
    }

    int n1 = N / 2;

    int n2 = N - n1;

    // Allocate new lists

    int *list1 = (int *)malloc(n1 * sizeof(int));
    int *list2 = (int *)malloc(n2 * sizeof(int));

    int i;

    for (i = 0; i < n1; i++)

        list1[i] = list_to_sort[i];

    for (i = 0; i < n2; i++)

        list2[i] = list_to_sort[n1 + i];

    // Sort list1 and list2

    pthread_mutex_lock(&m_thread_count);

    if(thread_count <= 14) { // we use threads if we have less than 14 threads, as 16 threads were the maximum.
        thread_count +=2;
        pthread_mutex_unlock(&m_thread_count);
        pthread_t thread1;
        pthread_t thread2;

        int *args1 = malloc(sizeof(int) * 2);
        args1[0] = list1;
        args1[1] = n1;
        int *args2 = malloc(sizeof(int) * 2);
        args2[0] = list2;
        args2[1] = n2;
        pthread_create(&thread1, NULL, thread_fn, args1);
        pthread_create(&thread2, NULL, thread_fn, args2);
        pthread_join(thread1, NULL);
        pthread_join(thread2, NULL);

    
    }
    else { // we do it sequentially if we have more than 14 threads
        pthread_mutex_unlock(&m_thread_count);
        merge_sort(list1, n1);
        merge_sort(list2, n2);
    }


    // Merge!

    int i1 = 0;

    int i2 = 0;

    i = 0;

    while (i1 < n1 && i2 < n2)
    {
        if (list1[i1] < list2[i2])
        {

            list_to_sort[i] = list1[i1];

            i1++;
        }

        else
        {

            list_to_sort[i] = list2[i2];

            i2++;
        }

        i++;
    }

    while (i1 < n1)
        list_to_sort[i++] = list1[i1++];

    while (i2 < n2)
        list_to_sort[i++] = list2[i2++];

    free(list1);
    free(list2);
}


int main(int argc, char* argv[]){
    int list[] = {3, 1, 4, 1, 5, 9, 2, 6, 5, 3};
    int N = sizeof(list) / sizeof(int);
    for (int i = 0; i < N; i++)
    {
        printf("%d ", list[i]);
    }
    printf("\n");
    merge_sort(list, N);
    for (int i = 0; i < N; i++)
    {
        printf("%d ", list[i]);
    }
    printf("\n");
    return 0;
}