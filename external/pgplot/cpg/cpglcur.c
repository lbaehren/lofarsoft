#include "cpgplot.h"
extern void pglcur_();

void cpglcur(int maxpt, int *npt, float *x, float *y)
{
  pglcur_(&maxpt, npt, x, y);
}
