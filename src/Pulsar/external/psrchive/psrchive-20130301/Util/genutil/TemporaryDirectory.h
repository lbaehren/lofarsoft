//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2012 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* Util/genutil/TemporaryDirectory.h */

#ifndef __TemporaryDirectory_h
#define __TemporaryDirectory_h

#include <string>

//! Creates a temporary directory for the current user
class TemporaryDirectory
{

 public:

  //! Create a temporary directory
  TemporaryDirectory (const std::string& basename);

  //! Return the temporary directory name
  const std::string& get_directory () const;

 private:

  //! The name of the temporary directory
  mutable std::string path;
  
  //! The basename for the temporary directory
  std::string base;

  void build () const;
};

#endif

