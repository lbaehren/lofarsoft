#include "cpgplot.h"
#include <string.h>
extern void pgconl_();

void cpgconl(const float *a, int idim, int jdim, int i1, int i2, int j1, int j2, float c, const float *tr, const char *label, int intval, int minint)
{
  int len_label = strlen(label);
  pgconl_((float *)a, &idim, &jdim, &i1, &i2, &j1, &j2, &c, (float *)tr, (char *)label, &intval, &minint, len_label);
}
