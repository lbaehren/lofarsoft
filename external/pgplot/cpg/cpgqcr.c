#include "cpgplot.h"
extern void pgqcr_();

void cpgqcr(int ci, float *cr, float *cg, float *cb)
{
  pgqcr_(&ci, cr, cg, cb);
}
