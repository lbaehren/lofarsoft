#include "cpgplot.h"
extern void pgerrb_();

void cpgerrb(int dir, int n, const float *x, const float *y, const float *e, float t)
{
  pgerrb_(&dir, &n, (float *)x, (float *)y, (float *)e, &t);
}
