#include "cpgplot.h"
extern void pgqvsz_();

void cpgqvsz(int units, float *x1, float *x2, float *y1, float *y2)
{
  pgqvsz_(&units, x1, x2, y1, y2);
}
