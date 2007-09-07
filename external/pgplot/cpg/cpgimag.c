#include "cpgplot.h"
extern void pgimag_();

void cpgimag(const float *a, int idim, int jdim, int i1, int i2, int j1, int j2, float a1, float a2, const float *tr)
{
  pgimag_((float *)a, &idim, &jdim, &i1, &i2, &j1, &j2, &a1, &a2, (float *)tr);
}
