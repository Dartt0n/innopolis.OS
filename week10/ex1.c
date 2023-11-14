#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>

#ifndef PATH_MAX
#define PATH_MAX FILENAME_MAX
#endif

char PATH[PATH_MAX];

void join_dir(char *path, const char *src)
{
    memset(path, 0, PATH_MAX);
    strcat(path, PATH);
    strcat(path, "/");
    strcat(path, src);
}

void find_all_hlinks(const char *source)
{
    DIR *dir;
    dir = opendir(PATH);
    if (dir == 0)
    {
        printf("failed to read directory\n");
        return;
    }
    char buf[PATH_MAX];

    struct stat src_stat;
    stat(source, &src_stat);

    char src_abs_path[PATH_MAX];
    strcpy(src_abs_path, source);

    for (struct dirent *cur_file; (cur_file = readdir(dir));)
    {
        struct stat cur_stat;
        join_dir(buf, cur_file->d_name);
        stat(buf, &cur_stat);

        char abs_path[PATH_MAX];
        strcpy(abs_path, buf);

        if (cur_stat.st_ino == src_stat.st_ino && strcmp(abs_path, src_abs_path))
        {
            printf("found hard link: <%s> inode: %d\n", abs_path, cur_stat.st_ino);
        }
    }
}

void unlink_all(const char *source)
{
    DIR *dir;
    dir = opendir(PATH);
    if (dir == 0)
    {
        printf("failed to read directory\n");
        return;
    }
    char buf[PATH_MAX];

    join_dir(buf, source);

    struct stat src_stat;
    stat(buf, &src_stat);

    char src_abs_path[PATH_MAX];
    realpath(buf, src_abs_path);

    char last[PATH_MAX];
    memset(last, 0, PATH_MAX);

    for (struct dirent *cur_file; (cur_file = readdir(dir));)
    {
        join_dir(buf, cur_file->d_name);
        struct stat cur_stat;
        stat(buf, &cur_stat);

        if (cur_stat.st_ino == src_stat.st_ino && strcmp(buf, src_abs_path))
        {
            if (strlen(last) != 0)
            {
                printf("remove <%s>\n", last);
                remove(last);
            }

            strcpy(last, buf);
        }
    }

    printf("last hard link: <%s>\n", last);
}

void create_sym_link(const char *source, const char *link)
{
    symlink(source, link);
}

void create_hard_link(const char *source, const char *_link)
{
    link(source, _link);
}

int main(int argc, char const *argv[])
{
    if (argc != 2)
    {
        printf("not enough arguments\n");
        return EXIT_FAILURE;
    }

    if (realpath(argv[1], PATH) == NULL)
    {
        printf("failed to resolve path %s\n", argv[1]);
        return EXIT_FAILURE;
    }

    char orig[PATH_MAX];
    char buf[PATH_MAX];
    join_dir(orig, "myfile1.txt");

    FILE *f = fopen(orig, "w");
    if (f == NULL)
    {
        printf("failed to open <%s> file\n", orig);
        return EXIT_FAILURE;
    }

    fprintf(f, "Hello world.");
    fclose(f);

    join_dir(buf, "myfile11.txt");
    create_hard_link(orig, buf);

    join_dir(buf, "myfile12.txt");
    create_hard_link(orig, buf);

    printf("After hard links created:\n");
    find_all_hlinks(orig);

    char *new_path = "/tmp/myfile1.txt";
    // if (rename(orig, new_path) < 0) // failed on my machine because /tmp is mounted on different device
    // {
    //     printf("failed to move <%s> to <%s>\n", orig, new_path);
    //     return EXIT_FAILURE;
    // }
    system("mv /home/dartt0n/opsys/week10/test/myfile1.txt /tmp/myfile1.txt");

    join_dir(buf, "myfile11.txt");
    f = fopen(buf, "w");
    if (f == NULL)
    {
        printf("failed to open <%s> file\n", buf);
        return EXIT_FAILURE;
    }
    fprintf(f, "Goodbye world.");
    fclose(f);

    join_dir(buf, "myfile13.txt");
    create_sym_link(new_path, buf);

    f = fopen(new_path, "w");
    if (f == NULL)
    {
        printf("failed to open <%s> file\n", new_path);
        return EXIT_FAILURE;
    }
    fprintf(f, "New world.");
    fclose(f);

    join_dir(buf, "myfile11.txt");

    printf("Before unlink:\n");
    find_all_hlinks(buf);
    unlink_all(buf);
    printf("After unlink:\n");
    find_all_hlinks(buf);

    return EXIT_SUCCESS;
}
