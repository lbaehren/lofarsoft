#include "cpgplot.h"
extern void pgqvp_();

void cpgqvp(int units, float *x1, float *x2, float *y1, float *y2)
{
  pgqvp_(&units, x1, x2, y1, y2);
}
