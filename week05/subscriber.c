#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>

#define MSGSIZE 1024

int main(int argc, char const *argv[])
{
    if (argc != 2)
    {
        return EXIT_FAILURE;
    }

    int n = atoi(argv[1]);

    char path[20];
    sprintf(path, "/tmp/ex1/s%d", n);

    char buffer[MSGSIZE];
    int readlen;
    for (;;)
    {
        int fd = open(path, 'r');
        memset(buffer, '\0', MSGSIZE);
        if (read(fd, buffer, MSGSIZE) > 0)
        {
            printf("%s\n", buffer);
        }
        close(fd);
    }

    return EXIT_SUCCESS;
}
