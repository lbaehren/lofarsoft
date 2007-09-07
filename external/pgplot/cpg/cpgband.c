#include "cpgplot.h"
#include <string.h>
extern int pgband_();

int cpgband(int mode, int posn, float xref, float yref, float *x, float *y, char *ch_scalar)
{
  int len_ch_scalar = 1;
  int r_value;
  r_value = pgband_(&mode, &posn, &xref, &yref, x, y, ch_scalar, len_ch_scalar);
  return r_value;
}
