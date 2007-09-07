#include "cpgplot.h"
extern void pgolin_();

void cpgolin(int maxpt, int *npt, float *x, float *y, int symbol)
{
  pgolin_(&maxpt, npt, x, y, &symbol);
}
