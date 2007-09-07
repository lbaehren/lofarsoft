#include "cpgplot.h"
extern void pggray_();

void cpggray(const float *a, int idim, int jdim, int i1, int i2, int j1, int j2, float fg, float bg, const float *tr)
{
  pggray_((float *)a, &idim, &jdim, &i1, &i2, &j1, &j2, &fg, &bg, (float *)tr);
}
