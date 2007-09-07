#include "cpgplot.h"
extern void pgslw_();

void cpgslw(int lw)
{
  pgslw_(&lw);
}
