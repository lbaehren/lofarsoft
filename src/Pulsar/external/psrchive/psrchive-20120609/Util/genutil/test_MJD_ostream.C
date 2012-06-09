/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/*
  This simple program tests to ensure that the MJD insertion operator
  behaves in the same way as that of the built-in types.
*/

#include "MJD.h"
#include "tostring.h"

using namespace std;

int main ()
{
  MJD mjd ("54321.098765432109876");

  double test = mjd.in_days();

  for (unsigned i=1; i<15; i++) {

    string mjd_string  = tostring(mjd,i);
    string test_string = tostring(test,i);

    cerr << "i=" << i << "\n"
	"    MJD=" << mjd_string << "\n"
	" double=" << test_string << endl;

    if (mjd_string != test_string) {
      cerr << "output mismatch" << endl;
      return -1;
    }

  }

  cerr << "MJD insertion operator passes all tests" << endl;

  return 0;

}
