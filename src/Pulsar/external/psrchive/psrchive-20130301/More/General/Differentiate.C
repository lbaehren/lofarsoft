/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Differentiate.h"
#include "Pulsar/Profile.h"

using namespace std;

Pulsar::Differentiate::Differentiate ()
{
  span = 1;
}

Pulsar::Differentiate::~Differentiate ()
{

}

void Pulsar::Differentiate::transform (Profile* profile)
{
  unsigned nbin = profile->get_nbin();
  float* amps = profile->get_amps();

  vector<float> difference (nbin);

  for (unsigned i=span; i<nbin+span; i++) {
    difference[i%nbin] = amps[(i+span)%nbin] - amps[i-span];
  }

  profile->set_amps (difference);
}
 
