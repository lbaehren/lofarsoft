#include "cpgplot.h"
extern void pgsubp_();

void cpgsubp(int nxsub, int nysub)
{
  pgsubp_(&nxsub, &nysub);
}
