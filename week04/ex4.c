#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>
#include <readline/readline.h>

int convert(char *string, char ***argv)
{
    int argc = 0;

    char *str = malloc((strlen(string) + 1) * sizeof(char));
    strcpy(str, string);

    for (char *token = strtok(string, " "); token != NULL; token = strtok(NULL, " "))
    {
        argc++;
    }

    strcpy(string, str);
    *argv = malloc((argc + 1) * sizeof(char *));

    int i = 0;
    for (char *token = strtok(string, " "); token != NULL; token = strtok(NULL, " "), i++)
    {
        (*argv)[i] = malloc((strlen(token) + 1) * sizeof(char));
        strcpy((*argv)[i], token);
    }

    (*argv)[i] = NULL;
    strcpy(string, str);

    free(str);

    return argc;
}

int main()
{
    pid_t parent_pid = getpid();

    char *argline = NULL;
    char **argv = malloc(sizeof(char *));
    size_t len = 0;
    int argc = 0;

    while (getpid() == parent_pid)
    {
        int n = getline(&argline, &len, stdin);
        argline[strlen(argline) - 1] = '\0';

        argc = convert(argline, &argv);

        if (n > 1)
        {
            free(argline);
            fork();
        }
    }

    execve(argv[0], argv, NULL);

    for (size_t i = 0; i < argc - 1; i++)
    {
        free(argv[i]);
    }
    free(argv);

    return EXIT_SUCCESS;
}
