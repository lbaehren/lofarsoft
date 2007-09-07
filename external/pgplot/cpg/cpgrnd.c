#include "cpgplot.h"
extern float pgrnd_();

float cpgrnd(float x, int *nsub)
{
  float r_value;
  r_value = pgrnd_(&x, nsub);
  return r_value;
}
