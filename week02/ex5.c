#include <stdlib.h>
#include <stdio.h>
#include <string.h>


long long int tribonacci(int n)
{
	int a = 0;
	int b = 1;
	int c = 1;

	// 0 1 1 2 4 7 13

	if (n == 0)
	{
		return a;
	} else if (n <= 2)
	{
		return b;
	}
	else for (int i = 0; i < n - 2; i++)
	{
		c = a + b + c;
		b = c - a - b;
		a = c - a - b;
	}

	return c;
}

int main()
{
	printf("%ld %ld\n", tribonacci(4), tribonacci(36));
	return 0;
}
