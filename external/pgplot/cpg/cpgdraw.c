#include "cpgplot.h"
extern void pgdraw_();

void cpgdraw(float x, float y)
{
  pgdraw_(&x, &y);
}
