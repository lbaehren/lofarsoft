/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/DeFaraday.h"

//! Default constructor
Pulsar::DeFaraday::DeFaraday ()
  : ColdPlasmaHistory ("DeFaraday")
{
}

//! Copy constructor
Pulsar::DeFaraday::DeFaraday (const DeFaraday& extension)
  : ColdPlasmaHistory ("DeFaraday")
{
  ColdPlasmaHistory::operator= (extension);
}

//! Operator =
const Pulsar::DeFaraday&
Pulsar::DeFaraday::operator= (const DeFaraday& extension)
{
  ColdPlasmaHistory::operator= (extension);
  return *this;
}
