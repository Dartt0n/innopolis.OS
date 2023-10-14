#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>

volatile sig_atomic_t SHOULDBREAK = 0;

void printer(int signum)
{
    FILE *f = fopen("./text.txt", "r");
    if (f == NULL)
    {
        printf("Failed to write file\n");
        return;
    }
    for (char buffer[1024]; fgets(buffer, sizeof(buffer), f);)
    {
        printf("%s", buffer);
    }
    fclose(f);
}
void breaker(int signum)
{
    printf("Process terminating...\n");
    SHOULDBREAK = 1;
}

int main(int argc, char const *argv[])
{
    // Save pid
    FILE *f = fopen("/var/run/agent.pid", "w");
    if (f == NULL)
    {
        printf("Failed to write file\n");
        return EXIT_FAILURE;
    }
    fprintf(f, "%d", getpid());
    fclose(f);

    // Print file content
    printer(0);

    if (signal(SIGUSR1, printer) == SIG_ERR)
    {
        printf("Failed to register handler for SIGUSR1\n");
        return EXIT_FAILURE;
    }
    if (signal(SIGUSR2, breaker) == SIG_ERR)
    {
        printf("Failed to register handler for SIGUSR2\n");
        return EXIT_FAILURE;
    }

    for (; SHOULDBREAK != 1;)
    {
    }

    remove("/var/run/agent.pid");

    return EXIT_SUCCESS;
}
