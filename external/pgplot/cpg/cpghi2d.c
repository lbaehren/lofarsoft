#include "cpgplot.h"
extern void pghi2d_();

void cpghi2d(const float *data, int nxv, int nyv, int ix1, int ix2, int iy1, int iy2, const float *x, int ioff, float bias, Logical center, float *ylims)
{
  int l_center = center ? 1:0;
  pghi2d_((float *)data, &nxv, &nyv, &ix1, &ix2, &iy1, &iy2, (float *)x, &ioff, &bias, &l_center, ylims);
}
