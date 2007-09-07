#include "cpgplot.h"
#include <string.h>
extern void pgmtxt_();

void cpgmtxt(const char *side, float disp, float coord, float fjust, const char *text)
{
  int len_side = strlen(side);
  int len_text = strlen(text);
  pgmtxt_((char *)side, &disp, &coord, &fjust, (char *)text, len_side, len_text);
}
