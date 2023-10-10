#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct Thread
{
    pthread_t id;
    int i;
    char message[256];
} Thread;

void *run_in_thread(void *arguments)
{
    Thread thr = *(Thread *)arguments;
    printf("%s\n", thr.message);
    return NULL;
}

int main(int argc, char const *argv[])
{
    if (argc != 2)
    {
        return EXIT_FAILURE;
    }

    int n = atoi(argv[1]);

    Thread *threads = (Thread *)malloc(n * sizeof(Thread));

    for (int i = 0; i < n; i++)
    {
        threads[i].i = i;
        sprintf(threads[i].message, "Hello from thread %d", i);

        pthread_create(&threads[i].id, NULL, run_in_thread, &threads[i]);
        printf("Thread %d is created\n", i);
        pthread_join(threads[i].id, NULL);
    }

    free(threads);

    return EXIT_SUCCESS;
}
