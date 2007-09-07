#include "cpgplot.h"
extern void pgsitf_();

void cpgsitf(int itf)
{
  pgsitf_(&itf);
}
