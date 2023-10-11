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

pthread_mutex_t global_lock = PTHREAD_MUTEX_INITIALIZER;
int k = 0; // current number to check whether it is prime
int c = 0; // primes count
int n = 0;

int get_number_to_check()
{

    int ret = k;
    if (k != n)
    {
        k++;
    }
    return ret;
}

void increment_primes()
{
    c++;
}

void *check_primes(void *arg)
{
    while (1)
    {
        pthread_mutex_lock(&global_lock);
        int ki = get_number_to_check();
        pthread_mutex_unlock(&global_lock);

        if (ki == n)
        {
            break;
        }

        if (is_prime(ki))
        {
            pthread_mutex_lock(&global_lock);
            increment_primes();
            pthread_mutex_unlock(&global_lock);
        }
    }

    return NULL;
}

int main(int argc, char const *argv[])
{
    if (argc != 3)
    {
        return EXIT_FAILURE;
    }
    n = atoi(argv[1]);
    int m = atoi(argv[2]);

    pthread_t *threads = (pthread_t *)malloc(m * sizeof(pthread_t));

    for (int i = 0; i < m; i++)
    {
        pthread_create(&threads[i], NULL, check_primes, NULL);
    }

    for (int i = 0; i < m; i++)
    {
        pthread_join(threads[i], NULL);
    }

    free(threads);

    printf("Result: %d primes\n", c);

    return EXIT_SUCCESS;
}
