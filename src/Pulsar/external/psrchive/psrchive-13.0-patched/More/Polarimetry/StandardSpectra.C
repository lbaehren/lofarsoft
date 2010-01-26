/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/StandardSpectra.h"
#include "Pulsar/PolnProfileStats.h"

using namespace std;

//! Default constructor
/*! If specified, baseline and on-pulse regions are defined by select */
Calibration::StandardSpectra::StandardSpectra (const Pulsar::PolnProfile* pp)
{
  stats = new Pulsar::PolnSpectrumStats;

  if (pp)
    select_profile (pp);
}

//! Select the baseline and on-pulse regions from profile
void
Calibration::StandardSpectra::select_profile (const Pulsar::PolnProfile* pp)
{
  stats->select_profile (pp);
  total_determinant = stats->get_total_determinant ();
}

//! Set the profile from which estimates will be derived
void
Calibration::StandardSpectra::set_profile (const Pulsar::PolnProfile* p) try
{
  stats->set_profile (p);

#ifdef _DEBUG
  cerr << "Calibration::StandardSpectra::set_profile onpulse nbin=" 
       << stats->get_real()->get_stats()->get_onpulse_nbin() << endl;
#endif

  total_determinant = stats->get_total_determinant ();
}
catch (Error& error)
{
  throw error += "Calibration::StandardSpectra::set_profile";
}

Pulsar::PolnSpectrumStats* Calibration::StandardSpectra::get_stats ()
{
  return stats;
}

unsigned Calibration::StandardSpectra::get_last_harmonic () const
{
  return stats->get_last_harmonic ();
}

const Pulsar::PolnProfile* Calibration::StandardSpectra::get_fourier () const
{
  return stats->get_fourier ();
}

//! Normalize estimates by the average determinant
void Calibration::StandardSpectra::set_normalize (bool norm)
{
  if (norm)
    normalize = new MEAL::NormalizeStokes;
  else
    normalize = 0;
}

//! Get the Stokes parameters of the specified phase bin
Stokes< std::complex< Estimate<double> > >
Calibration::StandardSpectra::get_stokes (unsigned ibin)
{
  Stokes< Estimate<double> > re = stats->get_real()->get_stokes (ibin);
  Stokes< Estimate<double> > im = stats->get_imag()->get_stokes (ibin);

#ifdef _DEBUG
  cerr << "ibin=" << ibin << endl;
  cerr << "re=" << re << endl;
  cerr << "im=" << im << endl;
#endif

  if (normalize)
  {
    normalize->normalize (re, total_determinant);
    normalize->normalize (im, total_determinant);

    unsigned nbin = stats->get_real()->get_stats()->get_onpulse_nbin();

    re *= sqrt( (double) nbin );
    im *= sqrt( (double) nbin );
  }

  std::complex< Estimate<double> > zero;
  Stokes< std::complex< Estimate<double> > > result( zero, zero, zero, zero );

  for (unsigned i=0; i<4; i++)
    result[i] = std::complex< Estimate<double> > (re[i], im[i]);

  return result;
}

