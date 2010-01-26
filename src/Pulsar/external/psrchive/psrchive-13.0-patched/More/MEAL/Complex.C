/***************************************************************************
 *
 *   Copyright (C) 2004-2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "MEAL/Complex.h"

/*! The class name is used in the output of template classes and methods */
const char* MEAL::Complex::Name = "Complex";

MEAL::Complex* MEAL::Complex::clone () const
{
  throw Error (InvalidState, "MEAL::Complex::clone", "not implemented");
}
