/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Alias.h"

using namespace std;

int main () try {

  Alias alias;

  alias.add ("hi", "there");
  alias.add ("a", "test");

  if (alias.get_name("hi") != "there") {
    cerr << "Alias::get_name fails" << endl;
    return -1;
  }

  if (alias.get_alias("test") != "a")  {
    cerr << "Alias::get_alias fails" << endl;
    return -1;
  }

  try {
    alias.get_name ("there");
    cerr << "Alias::get_name does not throw expected exception" << endl;
    return -1;
  }
  catch (Error& error) {
  }

  try {
    alias.get_alias ("something");
    cerr << "Alias::get_alias does not throw expected exception" << endl;
    return -1;
  }
  catch (Error& error) {
  }

  cerr << "Alias class passes all tests" << endl;

  return 0;
}
catch (Error& error) {
  cerr << "Unexpected exception: " << error << endl;
  return -1;
}

