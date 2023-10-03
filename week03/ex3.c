#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>

typedef struct Directory
{
    char name[64];
    struct File *files[256];
    struct Directory *directories[256];
    unsigned char nf;
    unsigned char nd;
    char path[2048];
} Directory;

typedef struct File
{
    size_t id;
    char name[64];
    size_t size;
    char data[1024];
    struct Directory *directory;
} File;

File new_file(char *name)
{
    File f;
    f.id = (size_t)&f; // unique number :)
    strcpy(f.name, name);
    strcpy(f.data, "");
    f.size = 0;

    return f;
}

void add_dir(Directory *p, Directory *c)
{
    if (p == NULL || c == NULL)
        return;

    p->directories[p->nd++] = c;

    strcpy(c->path, p->path);
    strcat(c->path, "/");
    strcat(c->path, c->name);
}

Directory new_dir(char *name)
{
    Directory d;

    strcpy(d.name, name);
    strcpy(d.path, "");
    d.nf = d.nd = 0;

    return d;
}

void overwrite_to_file(File *file, const char *str)
{
    strcpy(file->data, str);
    file->size = strlen(file->data) + 1;
}

void append_to_file(File *file, const char *str)
{
    strcat(file->data, str);
    file->size = strlen(file->data) + 1;
}

void printp_file(File *file)
{
    printf("%s/%s\n", file->directory->path, file->name);
}

void add_file(Directory *dir, File *file)
{
    dir->files[dir->nf++] = file;
    file->directory = dir;
}

void show_dir(Directory *d)
{
    printf("\nDIR\n");
    printf(" path: %s\n", d->path);
    printf(" files: [\n");
    for (int i = 0; i < d->nf; i++)
    {
        printf("%s,\n", d->files[i]->name);
    }
    printf(" ]\n");
    printf(" directories: [\n");
    for (int i = 0; i < d->nd; i++)
    {
        show_dir(d->directories[i]);
    }
    printf(" ]\n");
}

int main()
{
    Directory root = new_dir("/");
    Directory home = new_dir("home");
    add_dir(&root, &home);

    Directory bin = new_dir("bin");
    add_dir(&root, &bin);

    File ex31c = new_file("ex3_1.c");
    add_file(&home, &ex31c);
    overwrite_to_file(&ex31c, "int printf(const char * format, ...);\n");

    File ex32c = new_file("ex3_2.c");
    add_file(&home, &ex32c);
    append_to_file(&ex32c, "//This is a comment in C language\n");

    File bash = new_file("bash");
    add_file(&bin, &bash);
    append_to_file(&bash, "Bourne Again Shell!!\n");

    append_to_file(&ex31c, "int main() { printf(\"Hello World!\") }\n");

    printp_file(&ex31c);
    printp_file(&ex32c);
    printp_file(&bash);

    return EXIT_SUCCESS;
}