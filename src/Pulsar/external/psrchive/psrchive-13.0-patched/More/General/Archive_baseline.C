/***************************************************************************
 *
 *   Copyright (C) 2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Archive.h"
#include "Pulsar/Profile.h"

Pulsar::PhaseWeight* Pulsar::Archive::baseline () const try
{
  Reference::To<Archive> total = this->total();
  return total->get_Profile(0,0,0)->baseline ();
}
catch (Error& error)
{
  throw error += "Pulsar::Archive::baseline";
}
