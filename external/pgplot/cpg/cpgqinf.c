#include "cpgplot.h"
#include <string.h>
extern void pgqinf_();

void cpgqinf(const char *item, char *value, int *value_length)
{
  int len_item = strlen(item);
  int len_value = --(*value_length);
  pgqinf_((char *)item, value, value_length, len_item, len_value);
  value[*value_length] = '\0';
}
