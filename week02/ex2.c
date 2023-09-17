#include <stdlib.h>
#include <stdio.h>
#include <string.h>


int main(int argc, char *argv[])
{
  char *name = malloc(256);
  int size = 0;

  // the great power of for-loops
  for (int c = getc(stdin); c != EOF && c != '.'; name[size] = (char)c, size++, c = getc(stdin)) { }

  for (; size >= 0; printf("%c", name[size]), size--) { }
  printf("\n");

  free(name);
  return EXIT_SUCCESS;
}
