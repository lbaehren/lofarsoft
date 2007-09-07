#include "cpgplot.h"
#include <string.h>
extern void pglab_();

void cpglab(const char *xlbl, const char *ylbl, const char *toplbl)
{
  int len_xlbl = strlen(xlbl);
  int len_ylbl = strlen(ylbl);
  int len_toplbl = strlen(toplbl);
  pglab_((char *)xlbl, (char *)ylbl, (char *)toplbl, len_xlbl, len_ylbl, len_toplbl);
}
