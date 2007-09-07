#include "cpgplot.h"
#include <string.h>
extern void pgwedg_();

void cpgwedg(const char *side, float disp, float width, float fg, float bg, const char *label)
{
  int len_side = strlen(side);
  int len_label = strlen(label);
  pgwedg_((char *)side, &disp, &width, &fg, &bg, (char *)label, len_side, len_label);
}
