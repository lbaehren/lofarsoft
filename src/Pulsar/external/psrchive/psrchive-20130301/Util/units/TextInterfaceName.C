/***************************************************************************
 *
 *   Copyright (C) 2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "TextInterfaceName.h"

TextInterface::Name::Name ()
{
  reset ();
}

void TextInterface::Name::reset ()
{
  count=0;
  in_range = false;
  in_precision = false;
}

bool TextInterface::Name::valid (char c) const
{
  count ++;

  // variable names must start with an alphabetical character
  if (count == 1)
    return isalpha (c);

  // if inside a range, accept integers, commas, and dashes
  else if (in_range)
  {
    if (c == ']')
    {
      in_range = false;
      return true;
    }

    return isdigit (c) || c == '-' || c == ',';
  }

  // if inside a precision, accept integers
  else if (in_precision)
    return isdigit (c);

  // start of a range
  if (c == '[')
  {
    in_range = true;
    return true;
  }
  
  // start of a precision
  if (c == '%')
  {
    in_precision = true;
    return true;
  }

  // otherwise, any alphanumeric character, colon, or underscore
  return isalnum(c) || c == ':' || c == '_';
}
