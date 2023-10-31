#include "ex2.h"

size_t F = 0; // size of RAM
size_t P = 0; // size of DISK
size_t DISK_ACCESSES = 0;
String *RAM;  // array of strings, each with len()=8 (len(RAM)  = F)
String *DISK; // array of strings, each with lne()=8 (len(DISK) = P)
PTE *TABLE;   // page entry table

void init_PTE(PTE *memory)
{
    memory->valid = false;
    memory->frame = -1;
    memory->dirty = false;
    memory->referenced = 0;
}

void print_disk(const char *identifier)
{
    printf("-- DISK DUMP {%s}:\n", identifier);
    for (size_t i = 0; i < P; i++)
    {
        printf("   - page [%02d] content={%s}\n", i, DISK[i]);
    }
    printf("-- DISK DUMP {%s}\n", identifier);
}

void print_ram(const char *identifier)
{
    printf("-- RAM DUMP {%s}:\n", identifier);
    for (size_t i = 0; i < F; i++)
    {
        printf("   - frame [%02d] content={%s}\n", i, RAM[i]);
    }
    printf("-- RAM DUMP {%s}\n", identifier);
}

int empty(char *value)
{
    for (size_t i = 0; i < STRSIZE - 1; i++)
    {
        if (value[i] != 0)
        {
            return 0;
        }
    }

    return 1;
}

int find_free_frame()
{
    for (size_t i = 0; i < F; i++)
    {
        if (empty(RAM[i]))
        {
            return i;
        }
    }

    return -1;
}

void disk2ram(size_t disk_index, size_t page_index)
{
    strcpy(RAM[page_index], DISK[disk_index]);
    print_ram("page_load");
}

void ram2disk(size_t disk_index, size_t page_index)
{
    strcpy(DISK[disk_index], RAM[page_index]);
    print_disk("page_write");
}

int random_range(int lower, int upper)
{
    return (rand() % (upper - lower + 1)) + lower;
}

bool SIGUSR1_paused = true;
void SIGUSR1_resume(int _)
{
    SIGUSR1_paused = false;
}

int main(int argc, char const *argv[])
{
    if (argc != 3)
    {
        printf("provide 2 arguments: P - number of pages and F - number of frames\n");
        return EXIT_FAILURE;
    }

    P = atoi(argv[1]);
    F = atoi(argv[2]);

    DISK = malloc(P * sizeof(String));

    for (size_t i = 0; i < P; i++)
    {
        for (size_t j = 0; j < STRSIZE - 1; j++)
        {
            DISK[i][j] = random_range('a', 'z');
        }
        DISK[i][STRSIZE - 1] = 0;
    }
    print_disk("intial disk");

    RAM = calloc(F, STRSIZE);
    print_ram("initial ram");

    mkdir(PAGEFILEDIR, (mode_t)0777);

    int fd = open(PAGEFILE, O_RDWR | O_CREAT | O_TRUNC, (mode_t)0777);
    if (fd == -1)
    {
        printf("failed to open file %s\n", PAGEFILE);
        return EXIT_FAILURE;
    }
    if (ftruncate(fd, P * sizeof(PTE)) == -1)
    {
        printf("failed to truncate file %s\n", PAGEFILE);
        return EXIT_FAILURE;
    }

    TABLE = mmap(NULL, P * sizeof(PTE), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (TABLE == MAP_FAILED)
    {
        printf("failed to mmap file %s\n", PAGEFILE);
        return EXIT_FAILURE;
    }

    for (size_t i = 0; i < P; i++)
    {
        init_PTE(&TABLE[i]);
    }

    signal(SIGUSR1, SIGUSR1_resume);
    for (;;)
    {
        for (; SIGUSR1_paused;)
            ;
        printf("[received SIGUSR1 signal]\n");
        SIGUSR1_paused = true;

        int wanted_page_index = -1;
        for (size_t i = 0; i < P && wanted_page_index == -1; i++)
        {
            if (TABLE[i].referenced != 0 && !TABLE[i].valid)
            {
                wanted_page_index = i;
            }
        }

        if (wanted_page_index != -1)
        {
            int frame = find_free_frame();
            if (frame != -1)
            {
                printf("[load from disk page %02d to ram frame %02d]\n", wanted_page_index, frame);
                disk2ram(wanted_page_index, frame);
                DISK_ACCESSES++;
                TABLE[wanted_page_index].valid = true;
                TABLE[wanted_page_index].dirty = false;
                TABLE[wanted_page_index].frame = frame;
            }
            else
            {
                int victim = random_range(0, F - 1);
                printf("[victim frame is %02d]\n", victim);

                int page_index = -1;
                for (int i = 0; i < P; i++)
                {
                    if (TABLE[i].frame == victim)
                    {
                        page_index = i;
                        break;
                    }
                }
                printf("[victim frame %02d is in page %02d]\n", victim, page_index);

                if (TABLE[page_index].dirty)
                {
                    printf("[page %02d is dirty, save to disk]\n", page_index);
                    ram2disk(page_index, victim);
                    DISK_ACCESSES++;
                }

                printf("[nullify table entry at page %02d]\n", page_index);
                init_PTE(&TABLE[page_index]);
                printf("[nullify ram frame %02d]\n", victim);
                memset(RAM[victim], 0, STRSIZE);

                printf("[load from disk page %02d to ram frame %02d]\n", wanted_page_index, victim);
                disk2ram(wanted_page_index, victim);
                DISK_ACCESSES++;
                TABLE[wanted_page_index].valid = true;
                TABLE[wanted_page_index].dirty = false;
                TABLE[wanted_page_index].frame = victim;
            }

            kill(TABLE[wanted_page_index].referenced, SIGCONT);
        }
        else
        {
            printf("[cleaning up]\n");
            free(DISK);
            free(RAM);
            munmap(TABLE, P * sizeof(PTE));
            remove(PAGEFILE);

            break;
        }
    }

    printf("TOTAL NUMBER OF DISK OPS: %d\n", DISK_ACCESSES);
    printf("[pager terminate]\n");
    return EXIT_SUCCESS;
}
