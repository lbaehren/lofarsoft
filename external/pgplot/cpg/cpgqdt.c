#include "cpgplot.h"
#include <string.h>
extern void pgqdt_();

void cpgqdt(int n, char *type, int *type_length, char *descr, int *descr_length, int *inter)
{
  int len_type = --(*type_length);
  int len_descr = --(*descr_length);
  pgqdt_(&n, type, type_length, descr, descr_length, inter, len_type, len_descr);
  type[*type_length] = '\0';
  descr[*descr_length] = '\0';
}
