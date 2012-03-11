/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "escape.h"

#include <iostream>
using namespace std;

int main ()
{
  cerr << "testing escape; this should finish almost instantly" << endl;

  if (escape("try\\nthis") != "try\nthis")
  {
    cerr << "escape backslash n failed" << endl;
    return -1;
  }

  cerr << "escape function passes all tests" << endl;

  return 0;
}
