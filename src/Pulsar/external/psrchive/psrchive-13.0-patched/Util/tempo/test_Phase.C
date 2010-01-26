/***************************************************************************
 *
 *   Copyright (C) 2000 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Phase.h"
using namespace std;

int main ()
{
  Phase p1 (0.0);
  Phase p2 (34.5);

  p1 += 367.9845;
  cerr << "p1(367.9845):" << p1 << endl;

  p1 = p2;
  cerr << "p1(34.5):" << p1 << endl;

  p1 += p2;
  cerr << "p1(69):" << p1 << endl;

  return 0;
}
