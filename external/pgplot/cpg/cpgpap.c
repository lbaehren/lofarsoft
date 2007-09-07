#include "cpgplot.h"
extern void pgpap_();

void cpgpap(float width, float aspect)
{
  pgpap_(&width, &aspect);
}
