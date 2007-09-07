#include "cpgplot.h"
extern void pgerry_();

void cpgerry(int n, const float *x, const float *y1, const float *y2, float t)
{
  pgerry_(&n, (float *)x, (float *)y1, (float *)y2, &t);
}
