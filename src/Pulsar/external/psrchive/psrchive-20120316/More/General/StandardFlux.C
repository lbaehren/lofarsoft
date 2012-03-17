/***************************************************************************
 *
 *   Copyright (C) 2010 by Paul Demorest
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/StandardFlux.h"
#include "Pulsar/Profile.h"
#include "Pulsar/ProfileShiftFit.h"
#include "Pulsar/PhaseWeight.h"

#include <math.h>

Pulsar::StandardFlux::StandardFlux() 
{
  stdprof = NULL;
  stdfac = 0.0;
  sumstd = 0.0;
  sumstd2 = 0.0;
  fit_shift = true;
}

Pulsar::StandardFlux::~StandardFlux() 
{
}

void Pulsar::StandardFlux::set_standard(const Profile *p) 
{
  // Save pointer to standard
  stdprof = p;

  // Set up phase shift fit if needed
  if (fit_shift) {
    psf.set_standard(const_cast<Profile *>(stdprof.get()));
    psf.set_nharm(stdprof->get_nbin()/4);
    psf.set_error_method(ProfileShiftFit::Traditional_Chi2);
  }

  // Save various sums for reuse
  sumstd = stdprof->sum();
  sumstd2 = stdprof->sumsq();

  // Measure flux of standard for normalization
  Reference::To<PhaseWeight> base = stdprof->baseline();
  stdfac = sumstd/(double)stdprof->get_nbin() - base->get_mean().get_value();
}

Estimate<double> Pulsar::StandardFlux::get_flux(const Profile *p)
{
  if (stdprof==NULL)
    throw Error (InvalidState, "Pulsar::StandardFlux",
        "get_flux() called before standard is set");

  if (fit_shift) {

    // Fit for scale and shift using ProfileShiftFit class
    psf.set_Profile(const_cast<Profile *>(p));
    return psf.get_scale() * stdfac;

  } else {

    // Do a simple linear least squares fit between standard and data

    if (p->get_nbin() != stdprof->get_nbin())
      throw Error (InvalidState, "Pulsar::StandardFlux",
          "Profile and standard nbins do not match (prof=%d, std=%d)",
          p->get_nbin(), stdprof->get_nbin());

    unsigned nb = p->get_nbin();
    double sumdata = p->sum();
    double sumdata2 = p->sumsq();
    double xsum = 0.0;
    for (unsigned i=0; i<nb; i++) 
      xsum += p->get_amps()[i] * stdprof->get_amps()[i];

    double d = (double)nb*sumstd2 - sumstd*sumstd;
    double scale = ((double)nb*xsum - sumdata*sumstd) / d;
    double offset = (sumstd2*sumdata - sumstd*xsum) / d;

    // Uncertainty calculation
    double sse = 0.0;
    for (unsigned i=0; i<nb; i++) {
      double diff = p->get_amps()[i] - scale*stdprof->get_amps()[i] - offset;
      sse += diff*diff;
    }
    double sigma2 = sse / ((double)nb - 2.0);
    double scale_var = (double)nb*sigma2/d;

    // Return vals
    Estimate<double> f;
    f.set_value(scale * stdfac);
    f.set_variance(scale_var * stdfac * stdfac);
    return f;

  }
}
