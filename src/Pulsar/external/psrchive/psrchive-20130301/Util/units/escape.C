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
      switch (s[end+1])
	{
	case 'n':
	  s[end+1] = '\n';
	  break;

	case 't':
	  s[end+1] = '\t';
	  break;

	case 'r':
	  s[end+1] = '\r';
	  break;

        case '\\':
          end+=1;
          break;

	default:
	  // if the escape sequence is not recognized, then leave the \ alone
	  end+=1;
	  continue;
	}

      // remove the backslash (this line reached only if not default: continue)
      s.erase (end, 1);
    }

  return s;
}
