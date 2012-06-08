/***************************************************************************
 *
 *   Copyright (C) 2004-2011 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/StandardSNR.h"

void Pulsar::StandardSNR::set_standard (const Profile* profile)
{
  fit.choose_maximum_harmonic = true;
  fit.set_standard (profile);
}

float Pulsar::StandardSNR::get_snr (const Profile* profile)
{
  fit.set_Profile (profile);
  return fit.get_snr();
}    

