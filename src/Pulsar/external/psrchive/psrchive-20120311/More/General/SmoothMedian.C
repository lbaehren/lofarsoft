/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/SmoothMedian.h"
#include "Pulsar/Profile.h"

#include <algorithm>

using namespace std;

void Pulsar::SmoothMedian::transform (Profile* profile)
{
  unsigned nbin = profile->get_nbin();
  float* amps = profile->get_amps();

  unsigned width = (unsigned) get_bins (profile);

  if (width < 3)
    width = 3;
  else if (width % 2 == 0)
    width ++;

#ifdef _DEBUG
  cerr << "Pulsar::SmoothMedian::transform nbin=" << nbin 
       << " width=" << width << endl;
#endif

  unsigned middle = width/2;

  vector<float> result (nbin);
  vector<float> window (width);

  for (unsigned ibin=0; ibin<nbin; ibin++) {

    for (unsigned jbin=0; jbin<width; jbin++)
      window[jbin] = amps[((ibin+jbin+nbin)-middle)%nbin];

    std::nth_element (window.begin(), window.begin()+middle, window.end());
    result[ibin] = window[middle];

  }

  profile->set_amps(result);
}
