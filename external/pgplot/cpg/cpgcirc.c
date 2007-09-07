#include "cpgplot.h"
extern void pgcirc_();

void cpgcirc(float xcent, float ycent, float radius)
{
  pgcirc_(&xcent, &ycent, &radius);
}
