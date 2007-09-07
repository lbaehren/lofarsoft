#include "cpgplot.h"
extern void pgqwin_();

void cpgqwin(float *x1, float *x2, float *y1, float *y2)
{
  pgqwin_(x1, x2, y1, y2);
}
