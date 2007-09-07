#include "cpgplot.h"
#include <string.h>
extern void pgptxt_();

void cpgptxt(float x, float y, float angle, float fjust, const char *text)
{
  int len_text = strlen(text);
  pgptxt_(&x, &y, &angle, &fjust, (char *)text, len_text);
}
