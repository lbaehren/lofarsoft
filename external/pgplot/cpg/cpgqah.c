#include "cpgplot.h"
extern void pgqah_();

void cpgqah(int *fs, float *angle, float *barb)
{
  pgqah_(fs, angle, barb);
}
