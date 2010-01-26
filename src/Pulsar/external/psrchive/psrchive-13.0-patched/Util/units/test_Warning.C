/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Warning.h"

using namespace std;

int main ()
{
  string expected_output =
    "This is a warning; code 3\n"
    "This is a different message\n"
    "This is a warning; code * [repeated 3 times]\n";

  ostringstream output;

  {
    Warning warning (output);
    int three = 3;
    char a = 'a';
    
    warning << "This is a warning; code " << three << endl;
    warning << "This is a warning; code " << 4 << endl;
    warning << "This is a warning; code " << a << endl;

    warning << "This is a different message" << endl;
  }

  if (expected_output != output.str()) {
    cerr << "test_Warning ERROR\n"
      "output:\n"
	 << output.str() <<
      "not equal to expected output:\n"
	 << expected_output << endl;

    return -1;
  }

  cerr << "test_Warning: Warning class passes all tests" << endl;

  return 0;
}
