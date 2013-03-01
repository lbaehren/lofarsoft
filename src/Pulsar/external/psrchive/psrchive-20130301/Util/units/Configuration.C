/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Configuration.h"
#include "stringtok.h"
#include "Error.h"

#include <fstream>

using namespace std;

//! Construct from the specified file
Configuration::Configuration (const char* filename)
{
  find_count = 0;

  if (!filename)
    return;

  load (filename);
}

void Configuration::load (const string& filename) throw (Error)
{
  DEBUG("Configuration::load filename='" << filename << "'");

  ifstream input (filename.c_str());
  if (!input)
  {
    DEBUG("Configuration::load throwing exception of type Error");
    throw Error (FailedSys, "Configuration::load", "ifstream("+filename+")");
  }
 
  string line;
  
  while (!input.eof())
  {
    getline (input, line);
    line = stringtok (line, "#\n", false);  // get rid of comments

    if (!line.length())
      continue;

    // parse the key
    string key = stringtok (line, " \t");

    DEBUG("Configuration::load key=" << key);

    if (!line.length())
      continue;

    // parse the equals sign
    string equals = stringtok (line, " \t");

    if (equals != "=" || !line.length())
      continue;

    DEBUG("Configuration::load value=" << line);


    Entry* entry = find (key);
    if (entry)
    {
      DEBUG("Configuration::load over-ride " << key << " = " << line);
      entry->value = line;
    }
    else
    {
      DEBUG("Configuration::load new " << key << " = " << line);
      entries.push_back( Entry(key,line) );
    }

  }

  filenames.push_back( filename );
}


Configuration::Entry* Configuration::find (const string& key) const
{
  find_count ++;

  const_cast<Configuration*>(this)->load ();

  DEBUG("Configuration::find size=" << entries.size() << " key=" << key);

  for (unsigned i=0; i<entries.size(); i++)
  {
    if (entries[i].key == key)
    {
      DEBUG("Configuration::find value=" << entries[i].value);
      return const_cast<Entry*>( &(entries[i]) );
    }
  }

  DEBUG("Configuration::find " << key << " NOT FOUND");

  return 0;
}

