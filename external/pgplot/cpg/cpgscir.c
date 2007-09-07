#include "cpgplot.h"
extern void pgscir_();

void cpgscir(int icilo, int icihi)
{
  pgscir_(&icilo, &icihi);
}
