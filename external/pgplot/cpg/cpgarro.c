#include "cpgplot.h"
extern void pgarro_();

void cpgarro(float x1, float y1, float x2, float y2)
{
  pgarro_(&x1, &y1, &x2, &y2);
}
