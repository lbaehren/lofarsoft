/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "separate.h"

using namespace std;

void separate (string s, vector<string>& commands, const string& delimiters)
{
  string opener = delimiters + "[('\"";

  while (s.length()) {

    // search for the first instance of lim not enclosed in [ brackets ]
    string::size_type end = 0;

    while ( (end = s.find_first_of (opener, end) ) != string::npos ) {

      if (s[end] == '[')
	end = s.find (']', end+1);
      else if (s[end] == '(')
	end = s.find (')', end+1);
      else if (s[end] == '"')
	end = s.find ('"', end+1);
      else if (s[end] == '\'')
	end = s.find ('\'', end+1);
      else
	break;

    }

    // the first naked delimiter
    string command = s.substr (0, end);

    commands.push_back (command);

    end = s.find_first_not_of (delimiters, end);
    s.erase (0, end);
    
  }
}
