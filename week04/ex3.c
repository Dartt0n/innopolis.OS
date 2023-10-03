#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <sys/wait.h>
#include <malloc.h>

// spawn process from parent process
pid_t procspawn(pid_t parent)
{
    if (getpid() == parent)
    {
        fork();
        return getpid();
    }

    return -1;
}

int main(int argc, char const *argv[])
{
    if (argc != 2)
    {
        return EXIT_FAILURE;
    }
    size_t n = atoi(argv[1]);

    for (size_t i = 0; i < n; i++)
    {
        procspawn(getpid());
        sleep(5);
    }

    // wait for all children
    for (; wait(NULL) > 0;)
        ;

    return EXIT_SUCCESS;
}
