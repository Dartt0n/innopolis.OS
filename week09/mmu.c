#include "ex1.h"

bool SIGCONT_paused = true;
void SIGCONT_resume(int _)
{
    SIGCONT_paused = false;
}

void print_table(PTE *table, size_t size)
{
    log("│                             \n");
    log("│  ╭─────── table dump ──────╮\n");
    log("│  │                         │\n");
    for (size_t i = 0; i < size; i++)
    {
        log("│  ├──┬(%02d)                  │\n", i);
        log("│  │  ├─valid     %-8s   │\n", table[i].valid ? "true" : "false");
        log("│  │  ├─frame     %-8d   │\n", table[i].frame);
        log("│  │  ├─dirty     %-8s   │\n", table[i].dirty ? "true" : "false");
        log("│  │  ├─age       %-8d   │\n", table[i].age);
        log("│  │  ├─counter   %-8d   │\n", table[i].counter);
        log("│  │  ╰─ref       %-8d   │\n", table[i].referenced);
    }
    log("│  │                         │\n");
    log("│  ╰─────────────────────────╯\n");
    log("│                             \n");
}

int main(int argc, char const *argv[])
{
    if (argc < 4)
    {
        printf("not enought command line arguments\n");
        return EXIT_FAILURE;
    }
    int numpages = atoi(argv[1]);
    char *refstr = (char *)argv[2];
    pid_t pagerpid = atoi(argv[argc - 1]);

    log("┍╾─ mmu started with %d pages\n", numpages);

    log("┝╾─ reading file <%s>\n", PAGEFILE);
    int fd = open(PAGEFILE, O_RDWR);
    if (fd == -1)
    {
        log("┕╾─◆ failed to open file %s\n" PAGEFILE);
        return EXIT_FAILURE;
    }

    log("┝╾─ obtaning stats for file <%s>\n", PAGEFILE);
    struct stat statbuffer;
    if (fstat(fd, &statbuffer) == -1)
    {
        log("┕╾─◆ failed to obtain file stats\n");
        return EXIT_FAILURE;
    }

    size_t table_size = statbuffer.st_size / sizeof(PTE);

    log("┝╾─ mapping file <%s>\n", PAGEFILE);
    PTE *table = (PTE *)mmap(NULL, statbuffer.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (table == MAP_FAILED)
    {
        log("┕╾─◆ failed to mmap file <%s>\n", PAGEFILE);
        return EXIT_SUCCESS;
    }

    log("┝╾─ initial table\n");
    print_table(table, table_size);

    signal(SIGCONT, SIGCONT_resume);

    size_t page_fault = 0;
    size_t page_hit = 0;
    size_t page_request = 0;

    log("┝╾─ mmu start main loop\n");
    for (size_t i = 2; i < argc - 1; i++)
    {
        const char *token = argv[i];
        log("┝╾─ performing operation <%s>\n", token);
        char command = token[0];
        int index = atoi(&token[1]);
        log("┝╾─ page index <%02d>\n", index);

        table[index].referenced = getpid();
        if (!table[index].valid)
        {
            log("┝╾─ page fault: requested page not in ram\n");
            page_fault++;

            log("┝╾─ send signal SIGUSR1 to pager <%d>\n", pagerpid);
            kill(pagerpid, SIGUSR1);

            log("┝╾─ mmu waits for SIGCONT signal...\n");
            for (; SIGCONT_paused;) // wait for SIGCONT
                ;
            SIGCONT_paused = true;

            log("┝╾─ received SIGCONT signal\n");
        }
        else
        {
            page_hit++;
        }
        page_request++;

        if (command == 'R')
        {
            log("┝╾─ read page <%0d>\n", index);
        }
        else if (command == 'W')
        {
            log("┝╾─ write page <%0d>\n", index);
            table[index].dirty = true;
        }
        else
        {
        }

        print_table(table, table_size);
    }
    log("┝╾─ mmu finished all operations\n");

    log("┝╾─ unmapping file...\n");
    munmap(table, statbuffer.st_size);
    close(fd);
    log("┝╾─ sending final signal SIGUSR1 (exit) to pager <%d>\n", pagerpid);
    kill(pagerpid, SIGUSR1);

    log("┕╾─ mmu finished\n");

    printf("Hit ratio %d/%d (%lf)\n", page_hit, page_request, (double)page_hit / (double)page_request);
    printf("Miss ratio %d/%d (%lf)\n", page_fault, page_request, (double)page_fault / (double)page_request);

    return EXIT_SUCCESS;
}
