#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <float.h>


int main(int argc, char *argv[])
{

  int             i16var;
  unsigned short  u8var;
  signed long int i32var;
  float           f16var;
  double          f32var;

  i16var = INT_MAX;
  u8var  = 2*SHRT_MAX;
  i32var = LONG_MAX;
  f16var = FLT_MAX;
  f32var = DBL_MAX;

  printf("i16var: size = %ld bytes, max=%d \n", sizeof(i16var), i16var);
  printf("u8var : size = %ld bytes, max=%d\n", sizeof(u8var), u8var);
  printf("i32var: size = %ld bytes, max=%ld\n", sizeof(i32var), i32var);
  printf("f16var: size = %ld bytes, max=%f\n", sizeof(f16var), f16var);
  printf("f32var: size = %ld bytes, max=%lf\n", sizeof(f32var), f32var);


  return EXIT_SUCCESS;
}
