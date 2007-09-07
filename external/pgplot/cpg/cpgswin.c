#include "cpgplot.h"
extern void pgswin_();

void cpgswin(float x1, float x2, float y1, float y2)
{
  pgswin_(&x1, &x2, &y1, &y2);
}
