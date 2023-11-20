#include <stdio.h>
#include <dirent.h>

int main(int argc, char const *argv[])
{
    DIR *dir = opendir("/");
    if (dir == NULL)
    {
        printf("failed to open directory\n");
        return 1;
    }

    printf("root dir (\"/\") content:\n");
    for (struct dirent *entry = readdir(dir); entry != NULL; entry = readdir(dir))
    {
        printf("%s\n", entry->d_name);
    }

    closedir(dir);
    return 0;
}
