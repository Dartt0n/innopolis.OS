#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/mman.h>
#include <string.h>
#include <signal.h>

#define SIZE 14

int stopped = 0;
char *password;

void handler(int signum)
{
    stopped = 1;
    munmap(password, SIZE);
    // free(password);
}

int main(int argc, char const *argv[])
{
    FILE *f = fopen("/tmp/ex1.pid", "w");
    if (f == NULL)
    {
        printf("failed to open file /tmp/ex1.pid\n");
        return EXIT_FAILURE;
    }

    pid_t pid = getpid();
    if (fprintf(f, "%d\n", pid) <= 0)
    {
        printf("failed to write to file\n");
        return EXIT_FAILURE;
    }
    fclose(f);

    FILE *random = fopen("/dev/urandom", "r");
    if (random == NULL)
    {
        printf("failed to read file /dev/urandom\n");
        return EXIT_FAILURE;
    }

    // "pass:........\0";
    char *password = mmap(NULL, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, 0, 0);
    if (password == MAP_FAILED)
    {
        printf("failed to map memory\n");
        return EXIT_FAILURE;
    }

    // or malloc:
    // char *password = malloc(SIZE);

    size_t len = 0;
    strcpy(password, "pass:");
    len = 5;

    for (; len < 13;)
    {
        char c = fgetc(random);
        if (isprint(c))
        {
            password[len++] = c;
        }
    }

    signal(SIGINT, handler);

    for (; !stopped;)
        ;

    return EXIT_SUCCESS;
}