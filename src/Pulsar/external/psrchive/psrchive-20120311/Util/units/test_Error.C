/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Error.h"

using namespace std;

int throw1 ()  {

  try {

    try {
      cerr << "Throwing an exception" << endl;
      throw Error (Undefined, "throw1", "msg detail %d", 4);
    }
    catch (Error& error)  {
      error += "another function";
      throw error << " add to the message value=" << 3.4;
    }

  }
  catch (Error& error)  {
    cerr << "Exception caught:\n" << error << endl;
    return 0;
  }

  
  return -1;
}

int throw2 ()  {

  try {

    cerr << "Throwing a string exception" << endl;
    string error = "this is an error string";

    throw Error (Undefined, "throw2", error);

  }
  catch (Error& error)  {
    cerr << "Exception caught:\n" << error << endl;
    return 0;
  }

  return -1;
}

int main () {

  cerr << Error (Undefined, "main()", "4 = %d", 4).warning() << endl;

  if (throw1() < 0)
    return -1;

  if (throw2() < 0)
    return -1;

  return 0;
}

