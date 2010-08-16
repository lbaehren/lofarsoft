/***************************************************************************
 *
 *   Copyright (C) 2004-2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "MEAL/Complex2.h"

/*! The class name is used in the output of template classes and methods */
const char* MEAL::Complex2::Name = "Complex2";

MEAL::Complex2* MEAL::Complex2::clone () const
{
  throw Error (InvalidState, "MEAL::Complex2::clone", "not implemented");
}
