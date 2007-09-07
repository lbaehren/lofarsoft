#include "cpgplot.h"
#include <string.h>
extern void pgbox_();

void cpgbox(const char *xopt, float xtick, int nxsub, const char *yopt, float ytick, int nysub)
{
  int len_xopt = strlen(xopt);
  int len_yopt = strlen(yopt);
  pgbox_((char *)xopt, &xtick, &nxsub, (char *)yopt, &ytick, &nysub, len_xopt, len_yopt);
}
