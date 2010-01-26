/***************************************************************************
 *
 *   Copyright (C) 2005 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "ModifyRestore.h"

#include <iostream>
#include <string>

using namespace std;

int main ()
{
  int value = 4;

  {
    ModifyRestore<int> mod (value, 5);
    if (value != 5) {
      cerr << "ModifyRestore<int> constructor failure" << endl;
      return -1;
    }
  }

  if (value != 4) {
    cerr << "ModifyRestore<int> destructor failure" << endl;
    return -1;
  }

  string text = "this is a test";

  {
    ModifyRestore<string> mod (text, "changed to this");
    if (text != "changed to this") {
      cerr << "ModifyRestore<string> constructor failure" << endl;
      return -1;
    }
  }

  if (text != "this is a test") {
    cerr << "ModifyRestore<string> destructor failure" << endl;
    return -1;
  }

  return 0;

}
