#include <stdio.h>
#include <stdlib.h>
#include <string.h>

long int convert(long long int number, int s, int t)
{
    if (s < 2 || s > 10 || t < 2 || t > 10 || number < 0) {
        return -1;
    }

    long long int decimal = 0;
    for (int mult = 1; number > 0; mult *= s, number /= 10)
    {
        int digit = number % 10;
        if (digit >= s) {
            return -1;
        }

        decimal += digit * mult;
    }

    long long int result = 0;
    for (int mult = 1; decimal > 0; mult *= 10, decimal /= t)
    {
        result += (decimal % t) * mult;
    }

    return result;
}

int main(int argc, char *argv[]) {
    long long int number;
    int s, t;

    scanf("%ld", &number);
    scanf("%d", &s);
    scanf("%d", &t);
    

    long long int res;
    if ((res = convert(number, s, t)) == -1) {
        printf("cannot convert!");
    } else {
        printf("%d\n", res);
    }

    return EXIT_SUCCESS;
}
