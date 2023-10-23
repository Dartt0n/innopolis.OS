#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define SIZE 10000000

typedef unsigned int uint;

uint *memory = NULL;

void allocate_first_fit(uint adrs, size_t size)
{
    int best_candidate = 0;

    for (int i = 0; i < SIZE - size; i++)
    {
        char score = 1;
        for (int j = i; j < i + size; j++)
        {
            if (memory[j] == 0)
            {
                score++;
            }
            else
            {
                continue;
            }
        }

        if (score)
        {
            best_candidate = i;
            break;
        }
    }

    for (int x = best_candidate; x < best_candidate + size; x++)
    {
        memory[x] = adrs;
    }
}

void allocate_worst_fit(uint adrs, size_t size)
{
    int best_candidate = 0;
    int best_score = 0;

    for (int i = 0; i < SIZE; i++)
    {

        int score = 0;
        int current = i;
        for (; i < SIZE && memory[i] == (uint)0; i++)
        {
            score++;
        }

        if (score > best_score)
        {
            best_candidate = current;
            best_score = score;
        }
    }

    for (int x = best_candidate; x < best_candidate + size; x++)
    {
        memory[x] = adrs;
    }
}

void allocate_best_fit(uint adrs, size_t size)
{
    int best_candidate = 0;
    int best_score = -SIZE;

    for (int i = 0; i < SIZE; i++)
    {
        int score = size;
        int next = i;
        for (int j = i; j < SIZE && !memory[j]; j++)
        {
            score--;
            next = j;
        }

        if (score < 0 && score > best_score)
        {
            best_candidate = i;
            best_score = score;
        }

        i = next;
    }

    for (int x = best_candidate; x < best_candidate + size; x++)
    {
        memory[x] = adrs;
    }
}

void clear(uint adrs)
{
    size_t size = 0;
    int start = 0;
    char found = 0;

    for (int i = 0; i < SIZE; i++)
    {
        if (memory[i] == adrs)
        {
            if (found)
            {
                size++;
            }
            else
            {
                found = 1;
                start = i;
                size = 1;
            }
        }
    }

    for (int x = start; x < start + size; x++)
    {
        memory[x] = 0;
    }
}

double run(void func(uint, size_t))
{
    memory = (uint *)calloc(SIZE, sizeof(uint));

    FILE *f = fopen("./queries.txt", "r");
    char *line = NULL;

    size_t size;
    uint adrs;

    clock_t start = clock();
    for (; !feof(f) && getline(&line, &size, f) != -1 && strcmp(line, "end"); free(line), line = NULL)
    {
        char command[8];
        sscanf(line, "%s %d %d", command, &adrs, &size);

        if (!strcmp(command, "allocate"))
        {
            func(adrs, size);
        }
        else if (!strcmp(command, "clear"))
        {
            clear(adrs);
        }
    }
    clock_t end = clock();
    free(line);

    fclose(f);
    free(memory);
    return difftime(end, start) / 1000.0;
}

int main(int argc, char const *argv[])
{
    printf("First fit : %8.2f ms\n", run(allocate_first_fit));
    printf("Best  fit : %8.2f ms\n", run(allocate_best_fit));
    printf("Worst fit : %8.2f ms\n", run(allocate_worst_fit));
    return 0;
}
