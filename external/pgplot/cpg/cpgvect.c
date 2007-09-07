#include "cpgplot.h"
extern void pgvect_();

void cpgvect(const float *a, const float *b, int idim, int jdim, int i1, int i2, int j1, int j2, float c, int nc, const float *tr, float blank)
{
  pgvect_((float *)a, (float *)b, &idim, &jdim, &i1, &i2, &j1, &j2, &c, &nc, (float *)tr, &blank);
}
