#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>

bool is_prime(int n)
{
    if (n <= 1)
    {
        return false;
    }
    int i = 2;
    for (; i * i <= n; i++)
    {
        if (n % i == 0)
        {
            return false;
        }
    }
    return true;
}

int primes_count(int a, int b)
{
    int ret = 0;
    for (int i = a; i < b; i++)
    {
        if (is_prime(i) != 0)
        {
            ret++;
        }
    }
    return ret;
}

typedef struct prime_request
{
    int a, b;
} prime_request;

void *prime_counter(void *arg)
{
    prime_request req = *(prime_request *)arg;

    int *count = (int *)malloc(sizeof(int));
    *count = primes_count(req.a, req.b);

    return (void *)count;
}

int main(int argc, char const *argv[])
{
    if (argc != 3)
    {
        return EXIT_FAILURE;
    }
    int n = atoi(argv[1]);
    int m = atoi(argv[2]);

    prime_request *reqs = (prime_request *)malloc(m * sizeof(prime_request));
    pthread_t *threads = (pthread_t *)malloc(m * sizeof(pthread_t));

    for (int i = 0; i < m; i++)
    {
        reqs[i].a = (n / m) * i;
        reqs[i].b = (n / m) * (i + 1);
    }
    reqs[m - 1].b = n;

    for (int i = 0; i < m; i++)
    {
        pthread_create(&threads[i], NULL, prime_counter, (void *)&reqs[i]);
    }

    int result = 0;
    for (int i = 0; i < m; i++)
    {
        int *count;
        pthread_join(threads[i], (void **)&count);
        result += *(int *)count;
        free(count);
    }

    free(reqs);
    free(threads);

    printf("Result: %d primes\n", result);

    return 0;
}
