/***************************************************************************
 *
 *   Copyright (C) 2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Parameters.h"
#include "FilePtr.h"

//! Load from the named file
void Pulsar::Parameters::load (const std::string& filename) try
{
  FilePtr fptr (filename, "r");
  load (fptr);
}
catch (Error& error)
  {
    throw error += "Pulsar::Parameters::load";
  }

//! Unload to the named file
void Pulsar::Parameters::unload (const std::string& filename) const try
{
  FilePtr fptr (filename, "w");
  unload (fptr);
}
catch (Error& error)
  {
    throw error += "Pulsar::Parameters::unload";
  }

//! Return the pulsar name
std::string Pulsar::Parameters::get_name () const
{
  throw Error (InvalidState, "Pulsar::Parameters::get_name",
	       "not implemented");
}
