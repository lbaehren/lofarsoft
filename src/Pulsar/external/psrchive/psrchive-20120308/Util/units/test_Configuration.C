/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#define _DEBUG

#include "Configuration.h"
#include "Error.h"

#include <iostream>
#include <stdlib.h>

using namespace std;

class FakeParser
{
public:
  void parse (const std::string&) { }
  std::string empty () const { return ""; }
};

int main () try {

  string filename = "test.cfg";

  /*
    automake sets the environment variable 'srcdir' before calling
    this program during 'make check'.  This is useful when the build
    directory is not the source directory.
  */

  char* srcdir = getenv ("srcdir");
  if (srcdir)
    filename = srcdir + ("/" + filename);

  Configuration config (filename.c_str());

  double f1 = config.get<double>("F1", 5.0);

  if (f1 != 3.0)
  {
    cerr << "F1 not parsed from test.cfg" << endl;
    return -1;
  }

  double f2 = config.get<double>("F2", 5.0);

  if (f2 != 5.0)
  {
    cerr << "Failure to set to default" << endl;
    return -1;
  }

  Configuration::Parameter<double> test ("F1", &config, 6.0);

  cerr << "lazy evaluation test = " << test << endl;

  cerr << "All tests passed" << endl;

  Configuration::Parameter<double>::ParseLoader<FakeParser> fake_test;

  return 0;
}
catch (Error& error) {
  cerr << error << endl;
  return -1;
}

