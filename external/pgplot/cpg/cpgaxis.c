#include "cpgplot.h"
#include <string.h>
extern void pgaxis_();

void cpgaxis(const char *opt, float x1, float y1, float x2, float y2, float v1, float v2, float step, int nsub, float dmajl, float dmajr, float fmin, float disp, float orient)
{
  int len_opt = strlen(opt);
  pgaxis_((char *)opt, &x1, &y1, &x2, &y2, &v1, &v2, &step, &nsub, &dmajl, &dmajr, &fmin, &disp, &orient, len_opt);
}
