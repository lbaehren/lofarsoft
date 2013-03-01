/***************************************************************************
 *
 *   Copyright (C) 2004-2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "MEAL/Scalar.h"

/*! The class name is used in the output of template classes and methods */
const char* MEAL::Scalar::Name = "Scalar";

MEAL::Scalar* MEAL::Scalar::clone () const
{
  throw Error (InvalidState, "MEAL::Scalar::clone", "not implemented");
}
