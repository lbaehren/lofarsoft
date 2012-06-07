/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Error.h"
#include "strutil.h"

#include <stdlib.h>

using namespace std;

string expand (const string& filename)
{
  if (filename[0] != '~')
    return filename;

  static const char* home = getenv("HOME");

  if (!home)
    throw Error (FailedSys, "expand(string&)", "Cannot determine $HOME");

  if (filename[1] == '/')
    return home + filename.substr(1);

  string system_home = pathname (home);
  return system_home + filename.substr(1);
}
