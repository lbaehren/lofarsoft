/***************************************************************************
 *
 *   Copyright (C) 2005 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/ZeroPadShift.h"
#include "Pulsar/Profile.h"
#include "interpolate.h"

using namespace std;

Pulsar::Option<unsigned> 
Pulsar::ZeroPadShift::interpolate ("zero_pad_interpolate", 64);

/* This algorithm uses zero padding in the fourier domain to
   interpolate the cross correlation function in the time domain. The
   technique is described at:
  
   http://www.dspguru.com/howto/tech/zeropad2.htm 
*/
Estimate<double> Pulsar::ZeroPadShift::get_shift () const
{
  // First compute the standard cross correlation function:

  Reference::To<Pulsar::Profile> ptr = observation->clone();
  Reference::To<Pulsar::Profile> stp = standard->clone();

  // Remove the baseline
  *ptr -= ptr->mean(ptr->find_min_phase(0.15));

  // Perform the correlation
  ptr->correlate (standard);

  // Remove the baseline
  *ptr -= ptr->mean(ptr->find_min_phase(0.15));
  
  vector< Estimate<float> > correlation;

  const unsigned nbin = observation->get_nbin();
 
  for (unsigned i = 0; i < nbin; i++) {
    correlation.push_back(ptr->get_amps()[i]);
  }
  
  vector< Estimate<float> > interpolated;
  
  interpolated.resize(correlation.size() * interpolate);
  
  // Perform the zero-pad interpolation
  
  fft::interpolate(interpolated, correlation);
  
  // Find the peak of the correlation function
  
  float maxval = 0.0;
  float maxloc = 0.0;
  
  for (unsigned i = 0; i < interpolated.size(); i++) {
    if (interpolated[i].val > maxval) {
      maxval = interpolated[i].val;
      maxloc = float(i) / interpolate;
    }
  }
  
  // Error estimate (???)
  float ephase = 1.0 / float(nbin);
  
  double shift = double(maxloc) / double(nbin);
  
  if (shift < -0.5)
    shift += 1.0;
  else if (shift > 0.5)
    shift -= 1.0;
  
  return Estimate<double>(shift,ephase*ephase);
}
