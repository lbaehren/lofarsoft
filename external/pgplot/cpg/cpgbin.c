#include "cpgplot.h"
extern void pgbin_();

void cpgbin(int nbin, const float *x, const float *data, Logical center)
{
  int l_center = center ? 1:0;
  pgbin_(&nbin, (float *)x, (float *)data, &l_center);
}
