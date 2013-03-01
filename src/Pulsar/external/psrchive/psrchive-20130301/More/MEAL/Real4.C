/***************************************************************************
 *
 *   Copyright (C) 2006-2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "MEAL/Real4.h"

/*! The class name is used in the output of template classes and methods */
const char* MEAL::Real4::Name = "Real4";

MEAL::Real4* MEAL::Real4::clone () const
{
  throw Error (InvalidState, "MEAL::Real4::clone", "not implemented");
}
