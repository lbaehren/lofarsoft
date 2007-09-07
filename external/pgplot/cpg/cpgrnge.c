#include "cpgplot.h"
extern void pgrnge_();

void cpgrnge(float x1, float x2, float *xlo, float *xhi)
{
  pgrnge_(&x1, &x2, xlo, xhi);
}
