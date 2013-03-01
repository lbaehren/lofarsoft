/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/SmoothMean.h"
#include "Pulsar/Profile.h"

#include <math.h>

using namespace std;

void Pulsar::SmoothMean::transform (Profile* profile)
{
  unsigned nbin = profile->get_nbin();
  float* amps = profile->get_amps();

  float bin_width = get_bins (profile);

  if (bin_width <= 1.0)
    bin_width = 2.0;

  unsigned iwidth = (unsigned) ceil( bin_width );

  if (iwidth < 3)
    iwidth = 3;
  else if (iwidth % 2 == 0)
    iwidth ++;

  vector<float> weights (iwidth, 1.0);

  if (iwidth != bin_width) {
    float half_remainder = 0.5 * (bin_width - iwidth + 2);
    weights[0] = weights[iwidth-1] = half_remainder;
  }

  unsigned middle = iwidth / 2;

  vector<float> result (nbin);

  for (unsigned ibin=0; ibin<nbin; ibin++) {

    double total = 0.0;

    for (unsigned jbin=0; jbin<iwidth; jbin++)
      total += weights[jbin] * amps[((ibin+jbin+nbin)-middle)%nbin];

    result[ibin] = total / bin_width;

  }

  profile->set_amps( result );
}

//
//
//

Pulsar::SmoothMean* Pulsar::SmoothMean::clone () const
{
  return new SmoothMean (*this);
}

class Pulsar::SmoothMean::Interface : public Smooth::Interface
{
public:
  Interface (SmoothMean* instance) : Smooth::Interface (instance)
  {
  }

  std::string get_interface_name () const { return "mean"; }
};

//! Return a text interface that can be used to configure this instance
TextInterface::Parser* Pulsar::SmoothMean::get_interface ()
{
  return new Interface (this);
}
