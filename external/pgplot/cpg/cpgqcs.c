#include "cpgplot.h"
extern void pgqcs_();

void cpgqcs(int units, float *xch, float *ych)
{
  pgqcs_(&units, xch, ych);
}
