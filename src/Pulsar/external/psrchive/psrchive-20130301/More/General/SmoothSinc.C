/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/SmoothSinc.h"
#include "Pulsar/Profile.h"
#include "FTransform.h"
#include "Error.h"

#include <math.h>

using namespace std;

void Pulsar::SmoothSinc::transform (Profile* profile)
{
  unsigned nbin = profile->get_nbin();
  float* amps = profile->get_amps();

  // half of the spectrum will be redundant (real input)
  unsigned keep ( 0.5 / get_turns(profile) );

  if (keep > nbin)
    throw Error (InvalidParam, "Pulsar::SmoothSinc::transform",
		 "low pass filter keep=%u > nbin=%u", keep, nbin);

  vector< complex<float> > c_input( nbin );
  vector< complex<float> > c_spectrum( nbin );

  for (unsigned ibin = 0; ibin < nbin; ibin ++)
    c_input[ibin] = amps[ibin];

  FTransform::fcc1d(nbin, (float*)&(c_spectrum[0]), (float*)&(c_input[0]));

  for (unsigned ibin=1; ibin < nbin/2; ibin ++) {
    if (ibin > keep)
      c_spectrum[ibin] = 0.0;
    c_spectrum[nbin-ibin] = conj(c_spectrum[ibin]);
  }

  FTransform::bcc1d(nbin, (float*)&(c_input[0]), (float*)&(c_spectrum[0]));

  for (int ibin = 0; ibin<nbin ; ibin ++)
    amps[ibin] = c_input[ibin].real() / nbin;

}

//
//
//

Pulsar::SmoothSinc* Pulsar::SmoothSinc::clone () const
{
  return new SmoothSinc (*this);
}

class Pulsar::SmoothSinc::Interface : public Smooth::Interface
{
public:
  Interface (SmoothSinc* instance) : Smooth::Interface (instance)
  {
  }

  std::string get_interface_name () const { return "sinc"; }
};

//! Return a text interface that can be used to configure this instance
TextInterface::Parser* Pulsar::SmoothSinc::get_interface ()
{
  return new Interface (this);
}
