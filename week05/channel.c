#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

#define MSGSIZE 1024

int publisher(int wpipe)
{
    char buffer[MSGSIZE];
    char c;
    int i;

    scanf("%c", &c);
    for (i = 0; i < MSGSIZE && c != '\n'; scanf("%c", &c), i++)
    {
        buffer[i] = c;
    }
    buffer[i] = '\0';

    write(wpipe, buffer, sizeof(buffer));
    close(wpipe);

    return EXIT_SUCCESS;
}

int subscriber(int rpipe)
{
    char buffer[MSGSIZE];

    read(rpipe, buffer, MSGSIZE);

    printf("Message=%s\n", buffer);
}

pid_t spawn(pid_t parent)
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
    int rwp[2];
    pipe(rwp);

    pid_t parent_pid, publisher_pid, subscriber_pid;

    parent_pid = getpid();

    publisher_pid = spawn(parent_pid);
    subscriber_pid = spawn(parent_pid);

    pid_t current_pid = getpid();

    if (current_pid == publisher_pid)
    {
        publisher(rwp[1]);
    }
    else if (current_pid == subscriber_pid)
    {
        subscriber(rwp[0]);
    }
    else if (current_pid == parent_pid)
    {
        for (; wait(NULL) > 0;)
            ;
    }

    return EXIT_SUCCESS;
}
