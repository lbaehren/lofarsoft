#include "cpgplot.h"
#include <string.h>
extern void pgscrn_();

void cpgscrn(int ci, const char *name, int *ier)
{
  int len_name = strlen(name);
  pgscrn_(&ci, (char *)name, ier, len_name);
}
