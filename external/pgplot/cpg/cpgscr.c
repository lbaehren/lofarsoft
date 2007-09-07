#include "cpgplot.h"
extern void pgscr_();

void cpgscr(int ci, float cr, float cg, float cb)
{
  pgscr_(&ci, &cr, &cg, &cb);
}
