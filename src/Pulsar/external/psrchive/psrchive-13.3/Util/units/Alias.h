//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Util/units/Alias.h,v $
   $Revision: 1.2 $
   $Date: 2006/11/29 05:37:49 $
   $Author: straten $ */

#ifndef __Alias_h
#define __Alias_h

#include "Reference.h"

#include <map>
#include <string>

//! Stores keyword-value pairs
/*! The Alias class is essentially a wrapper around the std::map */
class Alias : public Reference::Able
{
public:

  //! Add an alias
  void add (const std::string& alias, const std::string& name);

  //! Given an alias, return the name
  std::string get_name (const std::string& alias) const;

  //! Given a word that may be an alias, return the associated name or the word
  std::string substitute (const std::string& alias) const;

  //! Given a name, return the alias
  std::string get_alias (const std::string& name) const;

protected:

  std::map<std::string, std::string> aliases;

};

#endif

