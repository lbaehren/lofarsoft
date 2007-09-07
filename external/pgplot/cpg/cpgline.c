#include "cpgplot.h"
extern void pgline_();

void cpgline(int n, const float *xpts, const float *ypts)
{
  pgline_(&n, (float *)xpts, (float *)ypts);
}
