/***************************************************************************
 *
 *   Copyright (C) 2012 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Convolve.h"
#include "Pulsar/Profile.h"

void Pulsar::Convolve::transform (Profile* profile)
{
  profile->convolve( get_operand() );
}
