/***************************************************************************
 *
 *   Copyright (C) 2004-2012 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "separate.h"

using namespace std;

bool isnumeric (char c)
{
  return isdigit(c) || c=='.' || c=='-';
}

bool part_of_numeric_range (const string& s, string::size_type pos)
{
  if (pos == 0 || pos+1 == s.length())
    return false;

  const string whitespaces (" \t\f\v\n\r");
  string::size_type next = s.find_first_not_of (whitespaces, pos+1);
	
  if (next == string::npos || !isnumeric(s[next]))
    return false;

  string::size_type last = s.find_last_not_of (whitespaces, pos-1);

  if (last == string::npos || !isnumeric(s[last]))
    return false;

  return true;
}

void separate (string s, vector<string>& commands, const string& delimiters)
{
  string brackets = "[{('\"";

  string opener = delimiters + brackets;

  while (s.length()) {

    /*
      search for the first instance of a delimiter that is

      1) not enclosed in brackets
      2) not part of a range of numbers

      nested brackets are not supported
    */

    string::size_type end = 0;

    while ( (end = s.find_first_of (opener, end) ) != string::npos )
    {

      if (s[end] == '[')
	end = s.find (']', end+1);

      else if (s[end] == '{')
	end = s.find ('}', end+1);

      else if (s[end] == '(')
	end = s.find (')', end+1);

      else if (s[end] == '"')
	end = s.find ('"', end+1);

      else if (s[end] == '\'')
	end = s.find ('\'', end+1);

      else if (part_of_numeric_range (s, end))
	end ++;

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

void standard_separation (vector<string>& list, const string& str)
{
  if (str.find (';') != string::npos)
    separate (str, list, ";");
  else
    separate (str, list, ",");
}

