#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CASESKIP 32

int lower(char *string)
{
	for (int i = 0; i < strlen(string); i++)
		if (string[i] >= 'A' && string[i] <= 'Z')
			string[i] += CASESKIP;

	return 0;
}

int count(char *string, char s)
{
	int count = 0;
	for (int i = 0; i < strlen(string); i++)
		if (string[i] == s)
			count++;

	return count;
}

int countAll(char *string)
{
	for (int i = 0; i < strlen(string)-1; i++)
		printf("%c:%d, ", string[i], count(string, string[i]));

	printf("%c:%d\n", string[strlen(string)-1], count(string, string[strlen(string)-1]));

	return 0;
}

int main()
{
	char string[256];
	fgets(string, 256, stdin);
	
	if ((strlen(string) > 0) && (string[strlen(string) - 1] == '\n'))
        	string[strlen(string) - 1] = '\0';
	
	lower(string);
	return countAll(string);
}
