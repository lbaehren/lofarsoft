/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/Dedisperse.h"

//! Default constructor
Pulsar::Dedisperse::Dedisperse ()
  : ColdPlasmaHistory ("Dedisperse")
{
}

//! Copy constructor
Pulsar::Dedisperse::Dedisperse (const Dedisperse& extension)
  : ColdPlasmaHistory ("Dedisperse")
{
  ColdPlasmaHistory::operator= (extension);
}

//! Operator =
const Pulsar::Dedisperse&
Pulsar::Dedisperse::operator= (const Dedisperse& extension)
{
  ColdPlasmaHistory::operator= (extension);
  return *this;
}
