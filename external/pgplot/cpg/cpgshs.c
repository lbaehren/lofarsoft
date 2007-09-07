#include "cpgplot.h"
extern void pgshs_();

void cpgshs(float angle, float sepn, float phase)
{
  pgshs_(&angle, &sepn, &phase);
}
