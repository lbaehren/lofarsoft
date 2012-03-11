/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Accumulate.h"
#include "Pulsar/Profile.h"

using namespace std;

Pulsar::Accumulate::Accumulate ()
{
  offset = 0;
  baseline = 0;
}

Pulsar::Accumulate::~Accumulate ()
{

}

void Pulsar::Accumulate::set_offset (unsigned o)
{
  offset = o;
}

unsigned Pulsar::Accumulate::get_offset () const
{
  return offset;
}

void Pulsar::Accumulate::set_baseline (double b)
{
  baseline = b;
}

double Pulsar::Accumulate::get_baseline () const
{
  return baseline;
}

void Pulsar::Accumulate::transform (Profile* profile)
{
  unsigned nbin = profile->get_nbin();
  float* amps = profile->get_amps();

  vector<double> cummulative (nbin);

  cummulative[0] = amps[offset] - baseline;

  for (unsigned i=1; i<nbin; i++)
    cummulative[i] = cummulative[i-1] + amps[(i+offset)%nbin] - baseline;

  profile->set_amps (cummulative);
}
 
