#include "cpgplot.h"
extern void pgsah_();

void cpgsah(int fs, float angle, float barb)
{
  pgsah_(&fs, &angle, &barb);
}
