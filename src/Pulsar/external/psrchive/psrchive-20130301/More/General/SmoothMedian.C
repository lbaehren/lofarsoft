/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/SmoothMedian.h"
#include "Pulsar/Profile.h"

#include "RunningMedian.h"

#include <algorithm>
#include <assert.h>

using namespace std;

#define CYCLIC_INCREMENT(i,n) i++; if (i==n) i = 0;

void Pulsar::SmoothMedian::transform (Profile* profile)
{
  const unsigned nbin = profile->get_nbin();
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
  unsigned front = 0;
  unsigned back = width;

  vector<float> result (nbin);

  RunningMedian<float> rmedian (middle);

  for (unsigned i=0; i<width; i++)
    rmedian.insert (amps[i]);

  for (unsigned ibin=0; ibin < nbin; ibin++)
  {
    result[middle] = rmedian.get_median();
    rmedian.insert (amps[back]);
    rmedian.erase (amps[front]);

    CYCLIC_INCREMENT(middle,nbin);
    CYCLIC_INCREMENT(front,nbin);
    CYCLIC_INCREMENT(back,nbin);
  }

  profile->set_amps(result);
}

//
//
//

Pulsar::SmoothMedian* Pulsar::SmoothMedian::clone () const
{
  return new SmoothMedian (*this);
}

class Pulsar::SmoothMedian::Interface : public Smooth::Interface
{
public:
  Interface (SmoothMedian* instance) : Smooth::Interface (instance)
  {
  }

  std::string get_interface_name () const { return "med"; }
};

//! Return a text interface that can be used to configure this instance
TextInterface::Parser* Pulsar::SmoothMedian::get_interface ()
{
  return new Interface (this);
}
