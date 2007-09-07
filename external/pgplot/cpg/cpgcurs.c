#include "cpgplot.h"
#include <string.h>
extern int pgcurs_();

int cpgcurs(float *x, float *y, char *ch_scalar)
{
  int len_ch_scalar = 1;
  int r_value;
  r_value = pgcurs_(x, y, ch_scalar, len_ch_scalar);
  return r_value;
}
