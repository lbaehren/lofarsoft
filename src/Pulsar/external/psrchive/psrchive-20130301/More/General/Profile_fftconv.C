/***************************************************************************
 *
 *   Copyright (C) 2005 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Profile.h"
#include "model_profile.h"

using namespace std;

void Pulsar::Profile::fftconv (const Profile& std, 
			       double& shift, float& eshift, 
			       float& snrfft, float& esnrfft) const
{
  double scale, sigma_scale;
  double dshift, sigma_dshift;
  double chisq;

  unsigned nbin = get_nbin();

  if (std.get_nbin() != nbin)
    throw Error (InvalidState, "Pulsar::Profile::fftconv",
		 "nbin=%u != std.nbin=%u", nbin, std.get_nbin());

  const float* obsamps = get_amps();
  const float* stdamps = std.get_amps();

  int ret = model_profile (nbin, 1, &obsamps, &stdamps, &scale, &sigma_scale, 
			   &dshift, &sigma_dshift, &chisq, verbose);
  
  if (ret != 0)
    throw Error (FailedCall, "Profile::fftconv", "model_profile failed");

  if (verbose) cerr << "Profile::fftconv"
		 " shift=" << dshift << " bins,"
		 " error=" << sigma_dshift <<
		 " scale=" << scale << 
		 " error=" << sigma_scale <<
		 " chisq=" << chisq << endl;

  double rms = sqrt( chisq / (float(nbin/2)-1.0) );

  shift = dshift;

  eshift = sigma_dshift;
  snrfft = 2 * sqrt( float(nbin) ) * scale / rms;
  esnrfft = snrfft * eshift / scale;

}

