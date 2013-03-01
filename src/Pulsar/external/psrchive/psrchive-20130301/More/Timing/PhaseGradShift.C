/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/PhaseGradShift.h"
#include "Pulsar/Profile.h"

using namespace std;

Estimate<double> Pulsar::PhaseGradShift::get_shift () const
{
  Profile stdcopy = *standard;
  Profile prfcopy = *observation;

  float snrfft = 0;
  float esnrfft = 999;

  // max float is of order 10^{38} - check that we won't exceed this
  // limiting factor in the DC term of the fourier transform

  if (stdcopy.sum() > 1e18)
    throw Error (InvalidState, "Profile::PhaseGradShift", 
		 "standard DC=%lf > max float", stdcopy.sum());

  if (prfcopy.sum() > 1e18)
    throw Error (InvalidState, "Profile::PhaseGradShift", 
		 "profile DC=%lf > max float", stdcopy.sum());

  const unsigned nbin = observation->get_nbin();

  if (Profile::verbose)
    cerr << "Profile::PhaseGradShift compare nbin="<< nbin
	 <<" "<< stdcopy.get_nbin() <<endl;

  if (nbin > stdcopy.get_nbin()) {
    if (nbin % stdcopy.get_nbin())
      throw Error (InvalidState, "Profile::PhaseGradShift", 
		   "profile nbin=%d standard nbin=%d",
                   nbin, stdcopy.get_nbin());

    unsigned nscrunch = nbin / stdcopy.get_nbin();
    prfcopy.bscrunch (nscrunch);
  }

  if (nbin < stdcopy.get_nbin()) {
    if (stdcopy.get_nbin() % nbin)
      throw Error (InvalidState, "Profile::PhaseGradShift", 
		   "profile nbin=%d standard nbin=%d",
                   nbin, stdcopy.get_nbin());

    unsigned nscrunch = stdcopy.get_nbin() / nbin;
    stdcopy.bscrunch (nscrunch);
  }

  double shift = 0.0;
  float eshift = 0.0;

  prfcopy.fftconv (stdcopy, shift, eshift, snrfft, esnrfft);
  double ephase = eshift / float(stdcopy.get_nbin());

  return Estimate<double> (shift / double(stdcopy.get_nbin()), ephase*ephase);

}


