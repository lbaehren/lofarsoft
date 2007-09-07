#include "cpgplot.h"
extern void pgwnad_();

void cpgwnad(float x1, float x2, float y1, float y2)
{
  pgwnad_(&x1, &x2, &y1, &y2);
}
