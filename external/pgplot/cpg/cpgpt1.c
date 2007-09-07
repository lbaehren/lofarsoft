#include "cpgplot.h"
extern void pgpt1_();

void cpgpt1(float xpt, float ypt, int symbol)
{
  pgpt1_(&xpt, &ypt, &symbol);
}
