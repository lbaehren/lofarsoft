/***************************************************************************
 *
 *   Copyright (C) 2000 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Cartesian.h"
using namespace std;

int main () {

  Cartesian c1 (9, 3, 11);
  Cartesian c2 (-4, -8, 15);

  cout << "c1=" << c1 << endl;
  cout << "c2=" << c2 << endl;

  cout << "min=" << min(c1,c2) << endl;
  cout << "max=" << max(c1,c2) << endl;

  return 0;
}
