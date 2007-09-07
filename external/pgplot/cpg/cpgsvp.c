#include "cpgplot.h"
extern void pgsvp_();

void cpgsvp(float xleft, float xright, float ybot, float ytop)
{
  pgsvp_(&xleft, &xright, &ybot, &ytop);
}
