#include "cpgplot.h"
#include <string.h>
extern void pglen_();

void cpglen(int units, const char *string, float *xl, float *yl)
{
  int len_string = strlen(string);
  pglen_(&units, (char *)string, xl, yl, len_string);
}
