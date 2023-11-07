#include "ex1.h"

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
    memory->counter = 0;
    memory->age = 0;
}

void print_disk()
{
    log("│                         \n");
    log("│  ╭────── disk dump ──────╮\n");
    log("│  │                       │\n");
    for (size_t i = 0; i < P; i++)
    {
        log("│  ├───(%04d) <%7s>    │\n", i, DISK[i]);
    }
    log("│  │                       │\n");
    log("│  ╰───────────────────────╯\n");
    log("│                         \n");
}

void print_ram()
{
    log("│                         \n");
    log("│  ╭─────── ram dump ──────╮\n");
    log("│  │                       │\n");
    for (size_t i = 0; i < F; i++)
    {
        log("│  ├───(%04d) <%7s>    │\n", i, RAM[i]);
    }
    log("│  │                       │\n");
    log("│  ╰───────────────────────╯\n");
    log("│                         \n");
}

int isempty(char *value)
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
        if (isempty(RAM[i]))
        {
            return i;
        }
    }

    return -1;
}

void disk2ram(size_t disk_index, size_t page_index)
{
    print_disk();
    print_ram();
    strcpy(RAM[page_index], DISK[disk_index]);
    log("┝╾─ ram after disk load operation:\n");
    print_ram();
}

void ram2disk(size_t disk_index, size_t page_index)
{
    print_disk();
    print_ram();
    strcpy(DISK[disk_index], RAM[page_index]);
    log("┝╾─ disk after disk write operation:\n");
    print_disk();
}

int random_range(int lower, int upper)
{
    return (rand() % (upper - lower + 1)) + lower;
}

void load_from_disk(int victim_frame, int wanted_page)
{
    log("┝╾─ loading disk page (%04d) to ram frame (%04d)\n", wanted_page, victim_frame);
    disk2ram(wanted_page, victim_frame);
    DISK_ACCESSES++;
    TABLE[wanted_page].valid = true;
    TABLE[wanted_page].dirty = false;
    TABLE[wanted_page].frame = victim_frame;
    log("┝╾─ loaded disk page (%04d) to ram frame (%04d)\n", wanted_page, victim_frame);
}

void prepare_ram(int page_index)
{
    log("┝╾─ preparing ram\n");
    print_ram();
    if (TABLE[page_index].dirty)
    {
        log("┝╾─ page (%04d) is dirty, save to disk\n", page_index);
        ram2disk(page_index, TABLE[page_index].frame);
        DISK_ACCESSES++;
    }

    log("┝╾─ nullifing ram frame (%04d)\n", TABLE[page_index].frame);
    memset(RAM[TABLE[page_index].frame], 0, STRSIZE);
    log("┝╾─ nullifing page (%04d)\n", page_index);
    TABLE[page_index].dirty = false;
    TABLE[page_index].frame = -1;
    TABLE[page_index].valid = false;
    log("┝╾─ prepared ram\n");
    print_ram();
}

int find_page(int frame)
{
    log("┝╾─ looking for corresponding disk page...\n");
    int page_index = -1;
    for (int i = 0; i < P; i++)
    {
        if (TABLE[i].frame == frame)
        {
            page_index = i;
            break;
        }
    }
    log("┝╾─ found corresponding disk page: (%04d)\n", page_index);
    return page_index;
}

int random_page()
{
    log("┝╾─ selecting victim page randomly...\n");
    int victim = random_range(0, F - 1);
    log("┝╾─ selected victim frame: (%04d)\n", victim);
    return find_page(victim);
}

int nfu()
{
    log("┝╾─ selecting victim page using NFU algorithm...\n");
    int victim = -1;
    int min_counter = __INT_MAX__;

    for (size_t i = 0; i < P; i++)
    {
        if (min_counter > TABLE[i].counter && TABLE[i].valid)
        {
            min_counter = TABLE[i].counter;
            victim = i;
        }
    }
    log("┝╾─ selected victim frame: (%04d)\n", TABLE[victim].frame);
    return victim;
}

int aging()
{
    log("┝╾─ selecting victim page using aging algorithm...\n");
    int victim = -1;
    int min_age = __UINT8_MAX__;

    for (size_t i = 0; i < P; i++)
    {
        if (min_age > TABLE[i].age && TABLE[i].valid)
        {
            min_age = TABLE[i].age;
            victim = i;
        }
    }

    log("┝╾─ selected victim frame: (%04d)\n", TABLE[victim].frame);
    return victim;
}

bool SIGUSR1_paused = true;
void SIGUSR1_resume(int _)
{
    SIGUSR1_paused = false;
}

int main(int argc, char const *argv[])
{
    if (argc != 4)
    {
        printf("provide 3 arguments: P - number of pages and F - number of frames, ALG - algorithm to use\n");
        return EXIT_FAILURE;
    }

    P = atoi(argv[1]);
    F = atoi(argv[2]);
    const char *algorimth = argv[3];
    int (*ALG)();
    if (strcmp(algorimth, "random") == 0)
    {
        ALG = random_page;
    }
    else if (strcmp(algorimth, "nfu") == 0)
    {
        ALG = nfu;
    }
    else if (strcmp(algorimth, "aging") == 0)
    {
        ALG = aging;
    }

    log("┍╾─ pager started with algorithm <%s>\n", algorimth);

    log("┝╾─ allocate disk\n");
    DISK = malloc(P * sizeof(String));

    for (size_t i = 0; i < P; i++)
    {
        for (size_t j = 0; j < STRSIZE - 1; j++)
        {
            DISK[i][j] = random_range('a', 'z');
        }
        DISK[i][STRSIZE - 1] = 0;
    }
    print_disk();

    log("┝╾─ allocate ram\n");
    RAM = calloc(F, STRSIZE);
    print_ram();

    log("┝╾─ create directory <%s>\n", PAGEFILEDIR);
    mkdir(PAGEFILEDIR, (mode_t)0777);

    log("┝╾─ create file <%s>\n", PAGEFILE);
    int fd = open(PAGEFILE, O_RDWR | O_CREAT | O_TRUNC, (mode_t)0777);
    if (fd == -1)
    {
        log("┕╾─◆ failed to create file <%s>\n", PAGEFILE);
        return EXIT_FAILURE;
    }

    log("┝╾─ truncate file <%s>\n", PAGEFILE);
    if (ftruncate(fd, P * sizeof(PTE)) == -1)
    {
        log("┕╾─◆ failed to truncate file <%s>\n", PAGEFILE);
        return EXIT_FAILURE;
    }

    log("┝╾─ map file <%s>\n", PAGEFILE);
    TABLE = mmap(NULL, P * sizeof(PTE), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (TABLE == MAP_FAILED)
    {
        log("┕╾─◆ failed to mmap file <%s>\n", PAGEFILE);
        return EXIT_FAILURE;
    }

    log("┝╾─ init table\n");
    for (size_t i = 0; i < P; i++)
    {
        init_PTE(&TABLE[i]);
    }

    signal(SIGUSR1, SIGUSR1_resume);

    log("┝╾─ pager start main loop\n");
    for (;;)
    {
        log("┝╾─ pager waits for SIGUSR1 signal...\n");
        for (; SIGUSR1_paused;)
            ;
        SIGUSR1_paused = true;
        log("┝╾─ received SIGUSR1 signal\n");

        int wanted_page_index = -1;
        pid_t mmu_pid;

        log("┝╾─ updating referenced pages...\n");
        log("┝╾─ looking for wanted page...\n");
        for (size_t i = 0; i < P; i++)
        {
            if (TABLE[i].referenced != 0 && !TABLE[i].valid && wanted_page_index == -1)
            {
                log("┝╾─ page (%04d) is wanted page\n");
                wanted_page_index = i;
                mmu_pid = TABLE[i].referenced;
            }

            TABLE[i].age >>= 1; // age the page - time spares no one

            if (TABLE[i].referenced != 0)
            {
                log("┝╾─ page (%04d) is referenced, increment counter, age & clear ref bit\n");
                TABLE[i].counter++;
                TABLE[i].referenced = 0;
                TABLE[i].age += 0b10000000; // set the first bit
            }
        }

        if (wanted_page_index != -1)
        {
            log("┝╾─ wanted page found: index (%04d)\n", wanted_page_index);

            log("┝╾─ looking for free frame...\n");
            int frame = find_free_frame();
            if (frame == -1)
            {
                log("┝╾─ free frame is not found, creating one\n");

                int page = ALG();

                prepare_ram(page);
                frame = find_free_frame();
                log("┝╾─ free frame is created: (%04d)\n", frame);
            }
            else
            {
                log("┝╾─ free frame is found: (%04d)\n", frame);
            }

            load_from_disk(frame, wanted_page_index);

            log("┝╾─ sent SIGCONT signal to <PID:%d>\n", mmu_pid);
            kill(mmu_pid, SIGCONT);
        }
        else
        {
            log("┝╾─ wanted page is not found\n");
            log("┝╾─ cleaning up...\n");
            log("┝╾─ free disk\n");
            free(DISK);
            log("┝╾─ free ram\n");
            free(RAM);
            log("┝╾─ unmap table feil\n");
            munmap(TABLE, P * sizeof(PTE));
            log("┝╾─ remove table file\n");
            remove(PAGEFILE);
            log("┝╾─ cleanup finished\n");
            log("┕╾─ pager finished main loop\n");

            break;
        }
    }

    // printf("TOTAL NUMBER OF DISK OPS: %d\n", DISK_ACCESSES);
    return EXIT_SUCCESS;
}
