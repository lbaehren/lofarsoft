#include "cpgplot.h"
extern void pgsls_();

void cpgsls(int ls)
{
  pgsls_(&ls);
}
