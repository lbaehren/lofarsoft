#include <math.h>
#include <stdio.h>

int main ()
{
  int i = 0;
  double x = 0;
  int e = 0;

  for (i=1; i < (1<<30); i*=2) {
    x = frexp ((double)i, &e);
    if (x != 0.5)  {
      fprintf (stderr, "%d is not a power of two?  x=%lf e=%d\n", i, x, e);
      return -1;
    }
  }
  return 0;
}

