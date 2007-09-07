#include "cpgplot.h"
#include <string.h>
extern int pgbeg_();

int cpgbeg(int unit, const char *file, int nxsub, int nysub)
{
  int len_file = strlen(file);
  int r_value;
  r_value = pgbeg_(&unit, (char *)file, &nxsub, &nysub, len_file);
  return r_value;
}
