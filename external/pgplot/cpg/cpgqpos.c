#include "cpgplot.h"
extern void pgqpos_();

void cpgqpos(float *x, float *y)
{
  pgqpos_(x, y);
}
