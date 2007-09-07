#include "cpgplot.h"
extern void pgpnts_();

void cpgpnts(int n, const float *x, const float *y, const int *symbol, int ns)
{
  pgpnts_(&n, (float *)x, (float *)y, (int *)symbol, &ns);
}
