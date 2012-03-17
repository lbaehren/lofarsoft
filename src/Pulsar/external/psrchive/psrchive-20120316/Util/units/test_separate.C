/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "separate.h"

#include <iostream>
using namespace std;

int main ()
{
  cerr << "testing separate; this should finish almost instantly" << endl;

  vector<string> test1;
  separate ("one=1, two,three, four=(4,5)", test1, ", ");

  if (test1.size() != 4) {
    cerr << "separate yields vector with only "
	 << test1.size() << " elements" << endl;
    return -1;
  }

  if (test1[0] != "one=1") {
    cerr << "separate fail test1[0]=" << test1[0] << endl;
    return -1;
  }

  if (test1[1] != "two") {
    cerr << "separate fail test1[1]=" << test1[1] << endl;
    return -1;
  }

  if (test1[2] != "three") {
    cerr << "separate fail test1[2]=" << test1[2] << endl;
    return -1;
  }

  if (test1[3] != "four=(4,5)") {
    cerr << "separate fail test1[3]=" << test1[3] << endl;
    return -1;
  }

  vector<string> test2;
  separate ("one \"two, three and four\"", test2);

  if (test2.size() != 2) {
    cerr << "separate yields vector with only "
	 << test2.size() << " elements" << endl;
    return -1;
  }

  if (test2[0] != "one") {
    cerr << "separate fail test2[0]=" << test2[0] << endl;
    return -1;
  }

  if (test2[1] != "\"two, three and four\"") {
    cerr << "separate fail test2[1]=" << test2[1] << endl;
    return -1;
  }

  cerr << "separate function passes all tests" << endl;

  return 0;
}
