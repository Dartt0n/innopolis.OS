#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/resource.h>
#include <unistd.h>
#include <time.h>

#define B *1
#define KB *1024 B
#define MB *1024 KB
#define GB *1024 MB

#define SIZE 200 MB

int main(int argc, char const *argv[])
{
    struct rusage usagebuffer;
    void *pointers[10];

    for (size_t i = 0; i < 10; i++)
    {
        pointers[i] = malloc(SIZE);
        memset(pointers[i], 0, SIZE);
        getrusage(RUSAGE_SELF, &usagebuffer);
        printf("Mem used: %ld\n", usagebuffer.ru_maxrss);
        sleep(1);
    }

    for (size_t i = 0; i < 10; i++)
    {
        free(pointers[i]);
    }

    return EXIT_SUCCESS;
}
