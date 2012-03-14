/***************************************************************************
 *
 *   Copyright (C) 2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "TextInterface.h"
#include "stringtok.h"
#include <stdio.h>

using namespace std;

void check_range (int i, int n, const char* name)
{
  if (i < 0)
    throw Error (InvalidRange, "TextInterface::parse_indeces",
		 "%s=%d < 0", name, i);

  if (i >= n)
    throw Error (InvalidRange, "TextInterface::parse_indeces",
		 "%s=%d >= size=%d", name, i, n);
}

bool valid_char (char c)
{
  return c == '-' || c == ',' || c == '+';
}

/*!  

  Defines the integer range parsing syntax used by a growing number of
  psrchive interfaces, especially those defined by the TextInterface
  family of classes.

  Valid ranges (where last = size - 1):

  "*"     = the empty set
  ""      = the full set (from 0 to last inclusive)
  "A"     = the integer A
  "A-"    = the range from A to last inclusive
  "A-B"   = the range from A to B inclusive
  "R+C"   = any non-empty range, with step size C
  "R1,R2" = combination of any non-empty ranges
*/

void TextInterface::parse_indeces (vector<unsigned>& index,
				   const string& name, unsigned size)
{
  if (name == "*")
    return;

  if (name == "")
  {
    index.resize (size);
    for (unsigned i=0; i<size; i++)
      index[i] = i;
    return;
  }

  string::size_type length = name.length();

  string range = name;
  
  // remove opening and closing braces used by TextInterface classes
  if (name[0] == '[' && name[length-1] == ']')
    range = name.substr (1,length-2);
  else if (name[0] == '[' || name[length-1] == ']')
    throw Error (InvalidParam, "TextInterface::parse_indeces",
		 "unbalanced brackets in '" + name + "'");

#ifdef _DEBUG
  cerr << "TextInterface::parse_indeces range started" << endl;
#endif

  if (range.empty())
    throw Error (InvalidParam, "TextInterface::parse_indeces",
		 "no range in '" + name + "'");

  for (unsigned i=0; i<range.size(); i++)
    if (!isdigit(range[i]) && !valid_char(range[i]))
      throw Error (InvalidParam, "TextInterface::parse_indeces",
		   "invalid range '" + range + "'");

  string backup = range;

  while (!range.empty())
  {
    string sub = stringtok (range, ",", false, false);

    if (range[0]==',')
      range.erase (0,1);

    unsigned increment = 1;

    string::size_type plus = sub.find ('+');
    if (plus != string::npos)
    {
      char dummy; // ensure that nothing follows the increment
      int scanned = sscanf (sub.c_str()+plus+1, "%u%c", &increment, &dummy);
      if (scanned != 1)
	throw Error (InvalidParam, "TextInterface::parse_indeces",
		     "invalid increment '%s'", sub.c_str()+plus);
      sub.erase (plus);
    }

    if (sub == "")
    {
      for (unsigned i=0; i<size; i+=increment)
	index.push_back(i);
      return;
    }

    int start, end;
    char c;

    int scanned = sscanf (sub.c_str(), "%d%c%d", &start, &c, &end);

    if (scanned == 3)
    {
      if (c != '-')
        throw Error (InvalidParam, "TextInterface::parse_indeces",
                     "invalid sub-range '" + range + "'");

#ifdef _DEBUG
      cerr << "TextInterface::parse_indeces start=" << start 
	   << " end=" << end << endl;
#endif

      check_range (start, size, "start");
      check_range (end, size, "end");

      if (end < start)
        throw Error (InvalidRange, "TextInterface::parse_indeces",
                     "end=%d < start=%d", end, start);

      for (int i=start; i <= end; i+=increment)
	index.push_back (i);
    }

    else if (scanned == 2)
    {
      if (c != '-')
        throw Error (InvalidParam, "TextInterface::parse_indeces",
                     "invalid sub-range '" + range + "'");

#ifdef _DEBUG
      cerr << "TextInterface::parse_indeces index=" << start << endl;
#endif

      check_range (start, size, "start");

      for (unsigned i=start; i < size; i+=increment)
	index.push_back (i);
    }

    else if (scanned == 1)
    {

#ifdef _DEBUG
      cerr << "TextInterface::parse_indeces index=" << start << endl;
#endif

      check_range (start, size, "start");

      index.push_back (start);
    }

    else
      throw Error (InvalidParam, "TextInterface::parse_indeces",
		                 "invalid sub-range '" + sub + "'");
  }

}
