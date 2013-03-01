/***************************************************************************
 *
 *   Copyright (C) 2010 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "MEAL/Complex4.h"

/*! The class name is used in the output of template classes and methods */
const char* MEAL::Complex4::Name = "Complex4";

MEAL::Complex4* MEAL::Complex4::clone () const
{
  throw Error (InvalidState, "MEAL::Complex4::clone", "not implemented");
}
