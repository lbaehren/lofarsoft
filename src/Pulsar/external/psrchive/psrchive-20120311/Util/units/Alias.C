/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Alias.h"

//! Add an alias
void Alias::add (const std::string& alias, const std::string& name)
{
  aliases[alias] = name;
}

//! Given an alias, return the name
std::string Alias::get_name (const std::string& alias) const
{
  std::map<std::string, std::string>::const_iterator it = aliases.find(alias);

  if (it == aliases.end())
    throw Error (InvalidParam, "Alias::get_name",
		 "no match for alias=" + alias);

  return it->second;
}

std::string Alias::substitute (const std::string& alias) const
{
  std::map<std::string, std::string>::const_iterator it = aliases.find(alias);

  if (it != aliases.end())
    return it->second;
  else
    return alias;
}

//! Given a name, return the alias
std::string Alias::get_alias (const std::string& name) const
{
  std::map<std::string, std::string>::const_iterator it;

  for (it = aliases.begin(); it != aliases.end(); it++)
    if (it->second == name)
      return it->first;

  throw Error (InvalidParam, "Alias::get_alias",
	       "no match for name=" + name);
}
