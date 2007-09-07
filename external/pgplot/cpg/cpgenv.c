#include "cpgplot.h"
extern void pgenv_();

void cpgenv(float xmin, float xmax, float ymin, float ymax, int just, int axis)
{
  pgenv_(&xmin, &xmax, &ymin, &ymax, &just, &axis);
}
