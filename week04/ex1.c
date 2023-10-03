#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <sys/wait.h>

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
    pid_t parent_pid = getpid();

    pid_t child1_pid = procspawn(parent_pid);
    pid_t child2_pid = procspawn(parent_pid);

    pid_t current_pid = getpid();
    if (current_pid == parent_pid)
    {
        printf("[%d] is a parent process, spawned from %d\n", current_pid, getppid());

        for (; wait(NULL) > 0;) // wait for all children
        {
        }
    }
    else if (current_pid == child1_pid)
    {
        printf("[%d] is a child 1 of [%d]\n", current_pid, getppid());
    }
    else if (current_pid == child2_pid)
    {
        printf("[%d] is a child 2 of [%d]\n", current_pid, getppid());
    }

    printf("[%d] finished in %.04f ms\n", current_pid, 1000.0f * (double)clock() / CLOCKS_PER_SEC);

    return EXIT_SUCCESS;
}
