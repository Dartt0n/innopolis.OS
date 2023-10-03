#include <stdio.h>
#include <stdlib.h>
#include <float.h>

// Fear and tremble, this is casthell!

void *aggregate(
    void *base,                               // array
    size_t size,                              // size of elements
    int n,                                    // length of array
    void *initial_value,                      // initial value for aggregation
    void *(*opr)(const void *, const void *)) // function
{
    if (size == sizeof(int))
    {
        int *output = (int *)malloc(size);
        *output = *(int *)initial_value;

        for (int i = 0; i < n; i++)
        {
            void *x = opr(output, &((int *)base)[i]);
            *output = *(int *)x;
            free(x);
        }

        return output;
    }
    else
    {
        double *output = (double *)malloc(size);
        *output = *(double *)initial_value;

        for (int i = 0; i < n; i++)
        {
            void *x = opr(output, &((double *)base)[i]);
            *output = *(double *)x;
            free(x);
        }

        return output;
    }
}

void *add_int(const void *a, const void *b)
{
    int *x = (int *)malloc(sizeof(int));
    *x = *(int *)a + *(int *)b;
    return x;
}

void *add_double(const void *a, const void *b)
{
    double *x = (double *)malloc(sizeof(double));
    *x = *(double *)a + *(double *)b;
    return x;
}

void *sub_int(const void *a, const void *b)
{
    int *x = (int *)malloc(sizeof(int));
    *x = *(int *)a - *(int *)b;
    return x;
}

void *sub_double(const void *a, const void *b)
{
    double *x = (double *)malloc(sizeof(double));
    *x = *(double *)a - *(double *)b;
    return x;
}

void *max_int(const void *a, const void *b)
{
    int *x = (int *)malloc(sizeof(int));
    *x = *(int *)a > *(int *)b ? *(int *)a : *(int *)b;
    return x;
}

void *max_double(const void *a, const void *b)
{
    double *x = (double *)malloc(sizeof(double));
    *x = *(double *)a > *(double *)b ? *(double *)a : *(double *)b;
    return x;
}

void *min_int(const void *a, const void *b)
{
    int *x = (int *)malloc(sizeof(int));
    *x = *(int *)a < *(int *)b ? *(int *)a : *(int *)b;
    return x;
}

void *min_double(const void *a, const void *b)
{
    double *x = (double *)malloc(sizeof(double));
    *x = *(double *)a < *(double *)b ? *(double *)a : *(double *)b;
    return x;
}

void *mul_int(const void *a, const void *b)
{
    int *x = (int *)malloc(sizeof(int));
    *x = *(int *)a * *(int *)b;
    return x;
}

void *mul_double(const void *a, const void *b)
{
    double *x = (double *)malloc(sizeof(double));
    *x = *(double *)a * *(double *)b;
    return x;
}

#define SIZE 5

const int ZERO = 0;
const int ONE = 1;
const double DOUBLE_MIN = DBL_MIN;

int main()
{
    int ints[SIZE] = {36, 64, 23, 5, 73};
    double doubles[SIZE] = {25.60, 34.52, 50.91, 77.90, 94.72};

    // print sum
    int *sm = aggregate(ints, sizeof(int), SIZE, (void *)&ZERO, add_int);
    printf("sum(ints) = %d\n", *sm);
    free(sm);

    // print sub
    double *sb = aggregate(doubles, sizeof(double), SIZE, (void *)&ZERO, sub_double);
    printf("sub(doubles) = %lf\n", *sb);
    free(sb);

    // print mul
    int *mi = aggregate(ints, sizeof(int), SIZE, (void *)&ONE, mul_int);
    printf("mul(ints) = %d\n", *mi);
    free(mi);

    // print max
    double *dm = aggregate(doubles, sizeof(double), SIZE, (void *)&DOUBLE_MIN, max_double);
    printf("max(doubles) = %lf\n", *dm);
    free(dm);

    return EXIT_SUCCESS;
}