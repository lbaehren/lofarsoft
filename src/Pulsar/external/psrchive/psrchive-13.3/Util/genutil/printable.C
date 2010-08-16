/***************************************************************************
 *
 *   Copyright (C) 2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "strutil.h"

#include <ctype.h>

// test that every character in a string is printable
bool printable (const std::string& s)
{
  size_t length = s.length();
  for (size_t i=0; i<length; i++)
    if ( !isprint(s[i]) )
      return false;

  return true;
}
