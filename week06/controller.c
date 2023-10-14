#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

int AGENT_PID = -1;
volatile sig_atomic_t SHOULDBREAK = 0;

void stopper(int _)
{
    if (AGENT_PID != -1)
    {
        kill(AGENT_PID, SIGTERM);
    }
    SHOULDBREAK = 1;
}

int main(int argc, char const *argv[])
{
    // Check if file exits
    if (access("/var/run/agent.pid", F_OK) != 0)
    {
        printf("Error: No agent found.\n");
        return EXIT_FAILURE;
    }
    else
    {
        FILE *f = fopen("/var/run/agent.pid", "r");
        fscanf(f, "%d", &AGENT_PID);
        fclose(f);
    }

    // Check if process exists
    if (kill(AGENT_PID, 0) == -1)
    {
        printf("Error: Agent pid is incorrect\n");
        return EXIT_FAILURE;
    }

    signal(SIGTERM, stopper);

    for (char buffer[1024]; SHOULDBREAK != 1;)
    {
        printf("Choose a command {\"read\", \"exit\", \"stop\", \"continue\"} to send to the agent\n");
        scanf("%1024s", buffer);

        if (strcmp("read", buffer) == 0)
        {
            kill(AGENT_PID, SIGUSR1);
        }
        else if (strcmp("exit", buffer) == 0)
        {
            kill(AGENT_PID, SIGUSR2);
            break;
        }
        else if (strcmp("stop", buffer) == 0)
        {
            kill(AGENT_PID, SIGSTOP);
        }
        else if (strcmp("continue", buffer) == 0)
        {
            kill(AGENT_PID, SIGCONT);
        }
        else
        {
            printf("Unknown command\n");
        }
    }

    return EXIT_SUCCESS;
}
