#include "ascii_header.h"
#include <stdio.h>

int main ()
{
  char header[4096] = 
  "VERSION 4.5         # the header version\n"
  "CALFREQ 1.234       # the modulation frequency of the diode\n"
  "FREQ    1400.5      # the radio frequency in MHz\n";

  double frequency = 0.0;
  float version = 0.0;
  double calfreq = 0.0;

  if (ascii_header_get (header, "GARBAGE", "%lf", &frequency) != -1)
  {
    fprintf (stderr, "test_ascii_header: failed to notice garbage\n");
    return -1;
  }

  if (ascii_header_get (header, "FREQ", "%lf", &frequency) < 0 || frequency != 1400.5)
  {
    fprintf (stderr, "test_ascii_header: failed to parse FREQ = %lf\n", frequency);
    return -1;
  }

  return 0;
}

