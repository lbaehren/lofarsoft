#include "cpgplot.h"
extern void pgctab_();

void cpgctab(const float *l, const float *r, const float *g, const float *b, int nc, float contra, float bright)
{
  pgctab_((float *)l, (float *)r, (float *)g, (float *)b, &nc, &contra, &bright);
}
