/***************************************************************************
 *
 *   Copyright (C) 2008 by Paul Demorest
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/FourierDomainFit.h"
#include "Pulsar/ProfileShiftFit.h"

using namespace std;

Estimate<double> Pulsar::FourierDomainFit::get_shift () const
{
  // TODO: figure out a way to reliably cache a standard profile
  // Do we really need to make copies here?
  Profile stdcopy = *standard;
  Profile prfcopy = *observation;

  Reference::To<Profile> std_p = &stdcopy;
  Reference::To<Profile> obs_p = &prfcopy;

  ProfileShiftFit fit;
  fit.set_standard(std_p);
  fit.set_nharm(std_p->get_nbin()/4);  // Use half the harmonics
  fit.set_error_method(ProfileShiftFit::MCMC_Variance); // How to make option?
  fit.set_Profile(obs_p);

  fit.compute();
  return fit.get_shift();
}


