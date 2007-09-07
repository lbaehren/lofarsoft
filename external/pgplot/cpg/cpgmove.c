#include "cpgplot.h"
extern void pgmove_();

void cpgmove(float x, float y)
{
  pgmove_(&x, &y);
}
