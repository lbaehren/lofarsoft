/***************************************************************************
 *
 *   Copyright (C) 2005 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "iri2rm.h"
#include <stdio.h>

int main ()
{
  double Glong = 150;
  double Glati = -33;
  float year = 2003;
  int mmdd = 220;
  double ut = 13.4;
  float az = -118;
  float el = 69;
  float RM = 0;
  
  iri2rm (&Glong, &Glati, &year, &mmdd, &ut, &az, &el, &RM);

  printf ("RM=%lf\n", RM);
  return 0;
}
