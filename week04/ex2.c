#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <sys/wait.h>
#include <malloc.h>

#define SIZE 120

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

typedef struct
{
    int *v;   // pointer to 1st array (to the 1st element)
    int *u;   // pointer to 2nd array (to the 1st element)
    size_t b; // slice begin
    size_t e; // slice end
} Args;

// maps function to `n` arguments in `n` processes spawned from `parent`
int procmapn(pid_t parent, int n, Args *args, void (*func)(Args))
{
    for (size_t i = 0; i < n; i++)
    {
        pid_t pid = procspawn(parent);

        if (pid != parent)
        {
            func(args[i]);
            break;
        }
    }

    return 0;
}

void dotproduct(Args args)
{
    FILE *f = fopen("temp.txt", "a");
    int r = 0;
    for (size_t i = args.b; i < args.e; i++)
    {
        r += args.v[i] * args.u[i];
    }
    fprintf(f, "%d ", r);
    fclose(f);
}

int main(int argc, char const *argv[])
{
    srand(time(NULL)); // random seed

    pid_t parent_pid = getpid();

    // fill vectors
    int u[SIZE], v[SIZE];
    for (size_t i = 0; i < SIZE; i++)
    {
        u[i] = rand() % 100;
        v[i] = rand() % 100;
    }

    // read n
    int n;
    printf("Enter the number of processes: ");
    scanf("%d", &n);
    printf("\n");

    // prepare data slices for multiprocesses call
    Args *args = (Args *)malloc(sizeof(Args) * n);
    for (size_t i = 0; i < n; i++)
    {
        args[i].v = v;
        args[i].u = u;
        args[i].b = (SIZE / n) * i;
        args[i].e = (SIZE / n) * (i + 1);
    }

    // call
    procmapn(parent_pid, n, args, dotproduct);

    if (getpid() == parent_pid)
    {
        // wait for all children
        for (; wait(NULL) > 0;)
            ;

        // collect results
        int r = 0;
        int t;
        FILE *f = fopen("temp.txt", "r");

        for (size_t i = 0; i < n; i++)
        {
            fscanf(f, "%d", &t);
            r += t;
        }

        // clean up
        free(args);
        fclose(f);
        remove("temp.txt");

        // answer
        printf("Dot product = %d\n", r);
    }

    return EXIT_SUCCESS;
}
