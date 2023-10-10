#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>

#define MSGSIZE 1024

pid_t spawn(pid_t parent)
{
    if (getpid() == parent)
    {
        fork();
        return getpid();
    }

    return -1;
}

int publish_proc(int pipe[2], int index)
{
    char path[20]; // max len for /tmp/ex1/s2147483647
    sprintf(path, "/tmp/ex1/s%d", index);
    int status = mkfifo(path, 0777);

    printf("[%06d] Mkfifo %s status %d\n", getpid(), path, status);

    close(pipe[1]);

    char buffer[MSGSIZE];
    for (;;)
    {
        memset(buffer, '\0', MSGSIZE); // clear buffer
        int n = read(pipe[0], buffer, MSGSIZE);
        int fd = open(path, O_WRONLY);
        write(fd, buffer, n);
        close(fd);
    }

    return EXIT_FAILURE;
}

int main_proc(int n, int (*pipes)[2])
{
    for (int i = 0; i < n; i++)
    {
        close(pipes[i][0]);
    }
    char buffer[MSGSIZE];
    for (;;)
    {
        int msglen = 0;

        char c;
        scanf("%c", &c);
        for (; c != '\n' && msglen < MSGSIZE; scanf("%c", &c), msglen++)
        {
            buffer[msglen] = c;
        }
        buffer[msglen] = '\0';

        for (int j = 0; j < n; j++)
        {
            write(pipes[j][1], buffer, msglen);
        }
    }

    return EXIT_SUCCESS;
}

int main(int argc, char const *argv[])
{
    if (argc != 2)
    {
        return EXIT_FAILURE;
    }
    int n = atoi(argv[1]);

    int(*pipes)[2] = malloc(n * sizeof(int[2]));
    for (int i = 0; i < n; i++)
    {
        pipe(pipes[i]);
    }

    mkdir("/tmp/ex1", 0777);

    pid_t main_pid = getpid();
    int index = 1;
    for (; index <= n; index++)
    {
        pid_t child_pid = spawn(main_pid);
        if (child_pid != main_pid)
        {
            break;
        }
    }

    if (getpid() == main_pid)
    {
        return main_proc(n, pipes);
    }
    else
    {
        return publish_proc(pipes[index - 1], index);
    }
}
