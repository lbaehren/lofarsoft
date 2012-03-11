/***************************************************************************
 *
 *   Copyright (C) 2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "tostring.h"
#include <math.h>

using namespace std;

int main ()
{
  for (double i=0; i < 5e6; i+=M_PI)
  {
    string s = tostring (i);
    i = fromstring<double> (s);
  }

  return 0;
}
