/***************************************************************************
 *
 *   Copyright (C) 2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "fitsutil.h"
#include "Error.h"

using namespace std;

int main () try
  {
    fits_version_check (true);
    return 0;
  }

 catch (Error& error)
   {
     cerr << "test_version: " << error << endl;
     return -1;
   }
