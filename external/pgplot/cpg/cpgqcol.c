#include "cpgplot.h"
extern void pgqcol_();

void cpgqcol(int *ci1, int *ci2)
{
  pgqcol_(ci1, ci2);
}
