#include "cpgplot.h"
extern void pgpixl_();

void cpgpixl(const int *ia, int idim, int jdim, int i1, int i2, int j1, int j2, float x1, float x2, float y1, float y2)
{
  pgpixl_((int *)ia, &idim, &jdim, &i1, &i2, &j1, &j2, &x1, &x2, &y1, &y2);
}
