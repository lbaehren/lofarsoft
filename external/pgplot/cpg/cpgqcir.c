#include "cpgplot.h"
extern void pgqcir_();

void cpgqcir(int *icilo, int *icihi)
{
  pgqcir_(icilo, icihi);
}
