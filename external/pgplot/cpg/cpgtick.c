#include "cpgplot.h"
#include <string.h>
extern void pgtick_();

void cpgtick(float x1, float y1, float x2, float y2, float v, float tikl, float tikr, float disp, float orient, const char *str)
{
  int len_str = strlen(str);
  pgtick_(&x1, &y1, &x2, &y2, &v, &tikl, &tikr, &disp, &orient, (char *)str, len_str);
}
