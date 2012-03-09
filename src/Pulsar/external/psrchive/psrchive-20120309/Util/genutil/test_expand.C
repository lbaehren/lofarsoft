/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "dirutil.h"
#include "Error.h"

using namespace std;

int main () try {

  string expanded = expand ("~/test");

  cout << "~ expanded filename = " << expanded << endl;

  expanded = expand ("~someone_else/test/again");

  cout << "~ user expanded filename = " << expanded << endl;

  return 0;

 }
 catch (Error& error) {
   cerr << error << endl;
   return -1;
 }
