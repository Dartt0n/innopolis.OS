#include <stdio.h>
#include <stdlib.h>

const int x = 1;

int const_tri(int *p, int n)
{
    if (n <= 2)
    {
        return p[n];
    }
    else
    {
        for (n -= 3; n > 0; n--)
        {
            p[2] = p[0] + p[1] + p[2];
            p[1] = p[2] - p[0] - p[1];
            p[0] = p[2] - p[0] - p[1];
        }
    }

    return p[2];
}

int main()
{
    int *q = &x;
    int *p = malloc(sizeof(int) * 3);
    p[0] = x;
    p[1] = x;
    p[2] = 2 * x;

    printf("q=%ld, p=%ld\n", q, p);

    if ((long long int)(p + 1) - (long long int)(p) == sizeof(int) &&
        (long long int)(p + 2) - (long long int)(p + 1) == sizeof(int))
    {
        printf("the cells are contiguous\n");
    }
    else
    {
        printf("the cells are contiguous\n");
    }

    int r = const_tri(p, 15);
    printf("15th tribonacci is %d\n", const_tri(p, 15));

    free(p);
    return EXIT_SUCCESS;
}