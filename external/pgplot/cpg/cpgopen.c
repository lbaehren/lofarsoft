#include "cpgplot.h"
#include <string.h>
extern int pgopen_();

int cpgopen(const char *device)
{
  int len_device = strlen(device);
  int r_value;
  r_value = pgopen_((char *)device, len_device);
  return r_value;
}
