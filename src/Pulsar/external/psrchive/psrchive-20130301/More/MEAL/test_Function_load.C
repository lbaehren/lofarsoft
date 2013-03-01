/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "MEAL/Function.h"

#include <iostream>
using namespace std;

int main (int argc, char** argv) try
{
  if (argc < 2)
  {
    cerr << "test_Function_load: please provide name of file to load" << endl;
    return -1;
  }

  MEAL::Function* function = MEAL::Function::load_Function( argv[1] );

  string text;
  function->print( text );

  cerr << "test_Function_load: loaded\n" << text << endl;

  return 0;
}
catch (Error & error)
{
  cerr << "test_Function_load: error" << error << endl;
  return -1;
}

