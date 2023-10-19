#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <ctype.h>

#define MAX_SIZE 524288000

void generate_file()
{
    size_t size = 0;

    int fdout;

    fdout = open("text.txt", O_RDWR | O_CREAT | O_TRUNC, (mode_t)0777);

    struct stat statbuf;
    fstat(fdout, &statbuf);

    lseek(fdout, MAX_SIZE - 1, SEEK_SET);
    write(fdout, "", 1) != 1;

    char *mapped = mmap(0, MAX_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fdout, 0);

    FILE *stream = fopen("/dev/random", "r");

    for (; size < MAX_SIZE;)
    {
        char c = fgetc(stream);
        if (isprint(c))
        {
            mapped[size++] = c;
        }

        if (size % 1024 == 0 && size < MAX_SIZE)
        {
            mapped[size++] = '\n';
        }
    }

    fclose(stream);
    munmap(mapped, MAX_SIZE);
    close(fdout);
}

char iscapital(char x)
{
    return x >= 'A' && x <= 'Z';
}

int count_capital()
{
    long sz = sysconf(_SC_PAGESIZE);
    size_t buffer_len = sz * 1024;
    size_t N = MAX_SIZE / buffer_len;

    int cap_number = 0;

    int fd = open("text.txt", O_RDONLY);
    for (int i = 0; i < N; i++)
    {
        char *src = mmap(0, buffer_len, PROT_READ, MAP_SHARED, fd, buffer_len * i);

        for (int j = 0; j < buffer_len; j++)
        {
            if (iscapital(src[j]))
            {
                cap_number++;
            }
        }

        munmap(src, buffer_len);
    }
    close(fd);

    return cap_number;
}

void file_to_lower()
{
    size_t size = 0;
    long sz = sysconf(_SC_PAGESIZE);
    size_t buffer_len = sz * 1024;

    int fd = open("text.txt", O_RDWR);

    struct stat statbuffer;
    fstat(fd, &statbuffer);
    size_t N = statbuffer.st_size / buffer_len;

    for (int i = 0; i < N; i++)
    {
        char *mapped = mmap(0, buffer_len, PROT_READ | PROT_WRITE, MAP_SHARED, fd, i * buffer_len);

        for (int j = 0; j < buffer_len; j++)
        {
            if (iscapital(mapped[j]))
            {
                mapped[j] = tolower(mapped[j]);
            }
        }

        munmap(mapped, buffer_len);
    }

    close(fd);
}

int main(int argc, char const *argv[])
{
    printf("Generating file...\n");
    generate_file();
    printf("File generated.\n");

    printf("Counting capital letters...\n");
    printf("Total number of capital letters: %d.\n", count_capital());

    printf("Replacing capital letters with lowercase ones...\n");
    file_to_lower();
    printf("Capital letters replaced.\n");

    printf("Counting capital letters...\n");
    printf("Total number of capital letters: %d.\n", count_capital());

    return EXIT_SUCCESS;
}
