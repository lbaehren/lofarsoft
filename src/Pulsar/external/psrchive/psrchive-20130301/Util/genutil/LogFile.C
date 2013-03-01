/***************************************************************************
 *
 *   Copyright (C) 2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "LogFile.h"
#include "stringtok.h"

#include <algorithm>
#include <fstream>

//! Construct from a filename
LogFile::LogFile (const std::string& filename)
{  
  std::ifstream input (filename.c_str());
  if (!input)
    throw Error (FailedSys, "LogFile ctor", "ifstream (" + filename + ")");

  std::string line;

  while (!input.eof())
  {
    std::getline (input, line);
    line = stringtok (line, "#\n", false);  // get rid of comments

    if (!line.length())
      continue;

    // the first key loaded should be the name of the instance
    std::string index = stringtok (line, " \t");

    add (index, line);
  }
}

//! add a log message
void LogFile::add (const std::string& index, const std::string& message)
{
  messages[index] = message;
}

bool compare (const std::string& a, const std::pair<std::string,std::string>& b)
{
  return a < b.first;
}

//! get a log message
std::string LogFile::get_message (const std::string& index)
{
  container::iterator place;

  place = std::upper_bound (messages.begin(), messages.end(), index, compare);
  
  if (place == messages.begin())
    throw Error (InvalidParam, "LogFile::get_message",
		 "no log message preceded '" + index + "'");

  place --;

  return place->second;
}

