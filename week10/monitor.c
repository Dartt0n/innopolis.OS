#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/inotify.h>
#include <signal.h>
#include <dirent.h>
#include <limits.h>
#include <fcntl.h>

#define EVENT_LEN sizeof(struct inotify_event) + FILENAME_MAX + 1

int SIGINT_exit = 1;
void SIGINT_handler(int _)
{
    SIGINT_exit = 0;
}

void print_stat(const char *path)
{
    DIR *dir;
    dir = opendir(path);
    if (dir == 0)
    {
        printf("failed to read directory\n");
        return;
    }

    printf("┌■\n");
    for (struct dirent *cur_file; (cur_file = readdir(dir));)
    {
        struct stat statbuf;
        fstat(open(cur_file->d_name, O_RDONLY), &statbuf);

        printf("├─┬──[%s]\n", cur_file->d_name);
        printf("│ ├─ file mode   %d\n", statbuf.st_mode);
        printf("│ ├─ file size   %d\n", statbuf.st_size);
        printf("│ ├─ link count  %d\n", statbuf.st_nlink);
        printf("│ ├─ last access %d\n", statbuf.st_atime);
        printf("│ ├─ last modify %d\n", statbuf.st_mtime);
        printf("│ └─ last status %d\n", statbuf.st_ctime);
        printf("│\n");
    }
    printf("└■\n");
}

int main(int argc, char const *argv[])
{
    if (argc != 2)
    {
        printf("not enough command line argument\n");
        return EXIT_FAILURE;
    }
    const char *path = argv[1];

    print_stat(path);

    int event_q = inotify_init1(IN_NONBLOCK);

    int dir_watch = inotify_add_watch(event_q, path, IN_ACCESS | IN_CREATE | IN_DELETE | IN_MODIFY | IN_OPEN | IN_ATTRIB);
    if (dir_watch == -1)
    {
        printf("failed to add watch\n");
        return EXIT_FAILURE;
    }

    char buffer[EVENT_LEN];
    signal(SIGINT, SIGINT_handler);
    for (SIGINT_exit = 1; SIGINT_exit;)
    {
        if (read(event_q, buffer, EVENT_LEN) < 0)
        {
            continue;
        }

        struct inotify_event *event = (struct inotify_event *)buffer;

        if (event->len)
        {
            int is_dir = event->mask & IN_ISDIR;

            const char *name;
            if (is_dir)
            {
                name = "directory";
            }
            else
            {
                name = "file";
            }

            if (event->mask & IN_ACCESS)
            {
                printf("%s <%s> was accessed\n", name, event->name);
            }

            if (event->mask & IN_CREATE)
            {
                printf("%s <%s> was created\n", name, event->name);
            }

            if (event->mask & IN_DELETE)
            {
                printf("%s <%s> was deleted\n", name, event->name);
            }

            if (event->mask & IN_MODIFY)
            {
                printf("%s <%s> was modified\n", name, event->name);
            }

            if (event->mask & IN_OPEN)
            {
                printf("%s <%s> was openned\n", name, event->name);
            }

            if (event->mask & IN_ATTRIB)
            {
                printf("%s <%s> metadata was changed\n", name, event->name);
            }
        }
    }

    inotify_rm_watch(event_q, dir_watch);
    print_stat(path);

    return EXIT_SUCCESS;
}
