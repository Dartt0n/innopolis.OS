#include <stdio.h>
#include <stdlib.h>
#include <math.h>

typedef struct
{
    double x;
    double y;
} Point;

float distance(Point *a, Point *b)
{
    return sqrt(pow(a->x - b->x, 2) + pow(a->y - b->y, 2));
}

float area(Point *a, Point *b, Point *c)
{
    return 0.5 * (a->x * b->y - b->x * a->y + b->x * c->y - c->x * b->y + c->x * a->y - a->x * c->y);
}

int main()
{
    Point A = {2.5, 6.0};
    Point B = {1.0, 2.2};
    Point C = {10.0, 6.0};

    printf("|AB| = %.04f\n", distance(&A, &B));
    printf("Area(ABC) = %.04f\n", area(&A, &B, &C));

    return EXIT_SUCCESS;
}