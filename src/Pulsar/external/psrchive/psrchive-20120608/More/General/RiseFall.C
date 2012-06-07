/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/RiseFall.h"
#include "Pulsar/PhaseWeight.h"
#include "Pulsar/Profile.h"

#include <assert.h>

// defined in Profile.C
void nbinify (int& istart, int& iend, int nbin);

//! Retrieve the PhaseWeight
void Pulsar::RiseFall::calculate (PhaseWeight* weight) 
{
  assert( weight != 0 );

  unsigned nbin = profile->get_nbin();
  weight->resize( nbin );
  weight->set_all( 0.0 );

  int bin_rise, bin_fall;
  get_indeces (bin_rise, bin_fall);

  nbinify (bin_rise, bin_fall, nbin);

  for (int ibin=bin_rise; ibin<bin_fall; ibin++)
    (*weight)[ibin % nbin] = 1.0;
}

std::pair<int,int> Pulsar::RiseFall::get_rise_fall (const Profile* profile)
{
  set_Profile (profile);

  int rise, fall;
  get_indeces (rise, fall);

  std::pair<int,int> result;
  result.first = rise;
  result.second = fall;

  return result;
}
