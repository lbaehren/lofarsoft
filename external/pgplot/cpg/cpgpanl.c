#include "cpgplot.h"
extern void pgpanl_();

void cpgpanl(int nxc, int nyc)
{
  pgpanl_(&nxc, &nyc);
}
