/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "escape.h"

using namespace std;

string escape (string s)
{
  string::size_type end = 0;

  while ( (end = s.find ('\\', end)) != string::npos && end+1 < s.length() )
    {
      s.erase (end, 1);

      switch (s[end])
	{
	case 'n':
	  s[end] = '\n';
	  break;

	case 't':
	  s[end] = '\t';
	  break;

	case 'r':
	  s[end] = '\r';
	  break;

	}

    }

  return s;
}
