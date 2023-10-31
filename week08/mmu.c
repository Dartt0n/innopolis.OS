#include "ex2.h"

bool SIGCONT_paused = true;
void SIGCONT_resume(int _)
{
    SIGCONT_paused = false;
}

void print_table(const char *identifier, PTE *table, size_t size)
{
    printf("-- PAGE TABLE DUMP {%s}:\n", identifier);
    for (size_t i = 0; i < size; i++)
    {
        printf("   - {.valid=%d, .frame=%d, .dirty=%d, .referenced=%d}\n", table[i].valid, table[i].frame, table[i].dirty, table[i].referenced);
    }
    printf("-- PAGE TABLE DUMP {%s}\n", identifier);
}

int main(int argc, char const *argv[])
{
    if (argc != 4)
    {
        printf("not enought command line arguments\n");
        return EXIT_FAILURE;
    }
    int numpages = atoi(argv[1]);
    char *refstr = (char *)argv[2];
    pid_t pagerpid = atoi(argv[3]);

    int fd = open(PAGEFILE, O_RDWR);
    if (fd == -1)
    {
        printf("failed to open file %s\n" PAGEFILE);
        return EXIT_FAILURE;
    }
    struct stat statbuffer;
    if (fstat(fd, &statbuffer) == -1)
    {
        printf("failed to obtain file stats\n");
        return EXIT_FAILURE;
    }

    size_t table_size = statbuffer.st_size / sizeof(PTE);

    PTE *table = (PTE *)mmap(NULL, statbuffer.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (table == MAP_FAILED)
    {
        printf("failed to map file %s\n", PAGEFILE);
        return EXIT_SUCCESS;
    }

    print_table("initial table", table, table_size);

    signal(SIGCONT, SIGCONT_resume);

    int iter = 0;
    for (char *token = strtok(refstr, " "); token != NULL; token = strtok(NULL, " "))
    {
        printf("[executing command %s]\n", token);
        char command = token[0];
        int index = atoi(&token[1]);

        if (!table[index].valid)
        {
            table[index].referenced = getpid();
            printf("[send signal SIGUSR1 to pager <%d>]\n", pagerpid);
            kill(pagerpid, SIGUSR1);

            for (; SIGCONT_paused;) // wait for SIGCONT
                ;
            SIGCONT_paused = true;
        }

        if (command == 'R')
        {
        }
        else if (command == 'W')
        {
            table[index].dirty = true;
        }
        else
        {
            printf("ignoted unknown command <%c>\n", command);
        }

        char id[14];
        sprintf(id, "iteration %03d", iter++);
        print_table(id, table, table_size);
    }
    printf("[mmu finished all operations]\n");

    munmap(table, statbuffer.st_size);
    close(fd);

    printf("[send signal SIGUSR1 to pager <%d>]\n", pagerpid);
    kill(pagerpid, SIGUSR1);
    printf("[mmu terminate]\n");

    return EXIT_SUCCESS;
}
