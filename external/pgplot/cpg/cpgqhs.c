#include "cpgplot.h"
extern void pgqhs_();

void cpgqhs(float *angle, float *sepn, float *phase)
{
  pgqhs_(angle, sepn, phase);
}
