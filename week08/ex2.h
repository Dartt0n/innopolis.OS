#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdbool.h>

#define PAGEFILEDIR "/tmp/ex2"
#define PAGEFILE "/tmp/ex2/pagetable" // The page table is held in memory backed by a file /tmp/ex2/pagetable
#define STRSIZE 8                     // 7 bytes + null termination symbol

typedef struct PTE
{
    bool valid;     // the page in the phisical memory (RAM)
    int frame;      // the frame number of the page in the RAM
    bool dirty;     // the page should be written to disk
    int referenced; // the page is referenced/requested
} PTE;

typedef char String[STRSIZE];