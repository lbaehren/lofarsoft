#include "cpgplot.h"
extern void pgncur_();

void cpgncur(int maxpt, int *npt, float *x, float *y, int symbol)
{
  pgncur_(&maxpt, npt, x, y, &symbol);
}
