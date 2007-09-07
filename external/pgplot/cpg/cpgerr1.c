#include "cpgplot.h"
extern void pgerr1_();

void cpgerr1(int dir, float x, float y, float e, float t)
{
  pgerr1_(&dir, &x, &y, &e, &t);
}
