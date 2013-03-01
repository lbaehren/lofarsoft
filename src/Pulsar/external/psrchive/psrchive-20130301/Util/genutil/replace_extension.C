/***************************************************************************
 *
 *   Copyright (C) 2004-2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include <string>
#include <cctype>

using namespace std;

// a simple command for replacing the extension on a filename
string replace_extension (const string& filename, const string& ext)
{
  string::size_type length = filename.length();

  if (ext.length() == 0 || length == 0)
    return filename;

  string::size_type index = filename.find_last_of (".", length);
  if (index == string::npos)
    index = length;
  else
  {
    // test if the extension is completely numerical
    string::size_type i=index+1;
    for (; i<length; i++)
      if ( !isdigit(filename[i]) )
	break;

    if (i == length)
      index = length;
  }

  string retval = filename.substr(0, index);

  if (ext[0] != '.')
    retval += ".";

  retval += ext;

  return retval;
}

