/***************************************************************************
 *
 *   Copyright (C) 2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/TextParameters.h"

using namespace std;
using namespace Pulsar;

template<typename T>
void run (TextParameters& test, const std::string& key, T expected)
{
  T value = test.get<T> (key);

  if (value != expected)
  {
    Error error (InvalidState, "test_TextParameters");
    error << "key=" << key << " got=" << value << " expected=" << expected;
    throw error;
  }
}

int main (int argc, char** argv) try
{
  // test Fortran-formatted float/double issue

  string text =
    "DM 5.67 \n"
    "F0 1.34D2 \n"
    "F1 -1.728314E-15";

  TextParameters test;
  test.set_text( text );

  run<double> (test, "DM", 5.67);
  run<float> (test, "DM", 5.67);

  run<double> (test, "F0", 134.0);
  run<float> (test, "F0", 134.0);

  run<double> (test, "F1", -1.728314e-15);
  run<float> (test, "F1", -1.728314e-15);

  cerr << "test_TextParameters: all tests passed" << endl;

  return 0;
}
 catch (Error& error)
   {
     cerr << "test_TextParameters: " << error << endl;
     return -1;
   }
