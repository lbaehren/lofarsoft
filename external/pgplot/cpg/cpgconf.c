#include "cpgplot.h"
extern void pgconf_();

void cpgconf(const float *a, int idim, int jdim, int i1, int i2, int j1, int j2, float c1, float c2, const float *tr)
{
  pgconf_((float *)a, &idim, &jdim, &i1, &i2, &j1, &j2, &c1, &c2, (float *)tr);
}
