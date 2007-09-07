#include "cpgplot.h"
#include <string.h>
extern void pgnumb_();

void cpgnumb(int mm, int pp, int form, char *string, int *string_length)
{
  int len_string = --(*string_length);
  pgnumb_(&mm, &pp, &form, string, string_length, len_string);
  string[*string_length] = '\0';
}
