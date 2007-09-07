#include "cpgplot.h"
extern void pgconb_();

void cpgconb(const float *a, int idim, int jdim, int i1, int i2, int j1, int j2, const float *c, int nc, const float *tr, float blank)
{
  pgconb_((float *)a, &idim, &jdim, &i1, &i2, &j1, &j2, (float *)c, &nc, (float *)tr, &blank);
}
