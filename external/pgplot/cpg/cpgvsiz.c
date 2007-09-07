#include "cpgplot.h"
extern void pgvsiz_();

void cpgvsiz(float xleft, float xright, float ybot, float ytop)
{
  pgvsiz_(&xleft, &xright, &ybot, &ytop);
}
