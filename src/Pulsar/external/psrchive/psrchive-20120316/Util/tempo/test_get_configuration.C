/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* A simple program to test the Tempo::get_configuration function */

#include "tempo++.h"

using namespace std;

int main () try
{
  vector<string> names;

  names.push_back("CLKDIR");
  names.push_back("PARDIR");
  names.push_back("OBS_NIST");
  names.push_back("UT1");
  names.push_back("EPHFILE");
  names.push_back("TDBFILE");

  for (unsigned i=0; i<names.size(); i++)
    cout << names[i] << " " << Tempo::get_configuration (names[i]) << endl;

  return 0;
}
catch (Error& error) {
  cerr << error << endl;
  return -1;
}
