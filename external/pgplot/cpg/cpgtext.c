#include "cpgplot.h"
#include <string.h>
extern void pgtext_();

void cpgtext(float x, float y, const char *text)
{
  int len_text = strlen(text);
  pgtext_(&x, &y, (char *)text, len_text);
}
