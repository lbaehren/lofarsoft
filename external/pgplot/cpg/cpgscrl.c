#include "cpgplot.h"
extern void pgscrl_();

void cpgscrl(float dx, float dy)
{
  pgscrl_(&dx, &dy);
}
