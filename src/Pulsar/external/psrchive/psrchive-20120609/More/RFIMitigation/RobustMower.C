/***************************************************************************
 *
 *   Copyright (C) 2012 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/RobustMower.h"
#include "Pulsar/PhaseWeight.h"
#include "Pulsar/Profile.h"

#include <algorithm>

void Pulsar::RobustMower::compute (PhaseWeight* mask,
				   const Profile* difference)
{
  const unsigned nbin = difference->get_nbin();
  const float* amps = difference->get_amps();

  std::vector<float> diff (nbin, 0.0);
  unsigned valid = 0;

  for (unsigned ibin=0; ibin<nbin; ibin++)
    if (difference->get_amps()[ibin] != 0.0)
    {
      diff[valid] = fabs( difference->get_amps()[ibin] );
      valid ++;
    }

#ifdef _DEBUG
  cerr << "nbin=" << nbin << " valid=" << valid << endl;
#endif

  std::nth_element (diff.begin(), diff.begin()+valid/2, diff.begin()+valid);

  float madm = diff[ valid/2 ];

  for (unsigned ibin=0; ibin<nbin; ibin++)
    (*mowed)[ibin] = ( fabs(amps[ibin]) > madm * cutoff_threshold );
}

