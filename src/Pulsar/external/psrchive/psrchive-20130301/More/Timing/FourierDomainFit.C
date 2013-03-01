/***************************************************************************
 *
 *   Copyright (C) 2008 by Paul Demorest
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/FourierDomainFit.h"
#include "Pulsar/ProfileShiftFit.h"

using namespace std;

Pulsar::FourierDomainFit::FourierDomainFit ()
{
  use_mcmc = true;
  reduced_chisq = 0;
}

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

  if (use_mcmc)
    fit.set_error_method(ProfileShiftFit::MCMC_Variance);
  else
    fit.set_error_method(ProfileShiftFit::Traditional_Chi2);

  fit.set_Profile(obs_p);

  fit.compute();

  reduced_chisq = fit.get_reduced_chisq();
  return fit.get_shift();
}

//! Return the statistical goodness of fit
double Pulsar::FourierDomainFit::get_reduced_chisq () const
{
  return reduced_chisq;
}

//
// The following Interface defines the pat command-line options
//

using Pulsar::FourierDomainFit;

// Text interface to the FourierDomainFit class
class FourierDomainFit::Interface : public TextInterface::To<FourierDomainFit>
{
public:
  Interface (FourierDomainFit* = 0);
  std::string get_interface_name () const { return "FDM"; }
};

FourierDomainFit::Interface::Interface (FourierDomainFit* instance)
{
  if (instance)
    set_instance (instance);

  add( &FourierDomainFit::get_mcmc,
       &FourierDomainFit::set_mcmc,
       "mcmc", "Use Markov chain Monte Carlo to estimate uncertainty");
}

TextInterface::Parser* FourierDomainFit::get_interface ()
{
  return new Interface(this);
}
