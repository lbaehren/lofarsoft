/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/PolnSpectrumStats.h"

#include "Pulsar/PolnProfileStats.h"
#include "Pulsar/PolnProfile.h"

#include "Pulsar/Fourier.h"
#include "Pulsar/LastHarmonic.h"
#include "Pulsar/BaselineEstimator.h"

using namespace std;

//! Default constructor
Pulsar::PolnSpectrumStats::PolnSpectrumStats (const PolnProfile* _profile)
{
  real = new Pulsar::PolnProfileStats;
  imag = new Pulsar::PolnProfileStats;

  regions_set = false;
  last_harmonic = 0;
  plan = 0;

  if (profile)
    select_profile (_profile);
}

//! Destructor
Pulsar::PolnSpectrumStats::~PolnSpectrumStats()
{
}

//! Set the PolnProfile from which statistics will be derived
void Pulsar::PolnSpectrumStats::set_profile (const PolnProfile* _profile)
{
  profile = _profile;
  build ();
}

//! Set the PolnProfile from which baseline and onpulse mask will be selected
/*! It is assumed that all subsequent PolnProfile instances passed to
  set_profile will have the same phase as set_profile */
void Pulsar::PolnSpectrumStats::select_profile (const PolnProfile* _profile)
{
  profile = _profile;
  regions_set = false;
  build ();
  if (profile)
    regions_set = true;
}

//! Set the on-pulse and baseline regions
void Pulsar::PolnSpectrumStats::set_regions (const PhaseWeight& on,
					     const PhaseWeight& off)
{
  real->set_regions (on, off);
  imag->set_regions (on, off);

  onpulse = on;
  baseline = off;

  regions_set = true;
  build ();
}

//! Set the on-pulse and baseline regions
void Pulsar::PolnSpectrumStats::get_regions (PhaseWeight& on, 
					     PhaseWeight& off) const
{
  on = onpulse;
  off = baseline;
}

//! Get the fourier transform of the last set profile
const Pulsar::PolnProfile* Pulsar::PolnSpectrumStats::get_fourier () const
{
  return fourier;
}

//! Get the Stokes parameters for the specified harmonic
Stokes< std::complex< Estimate<double> > > 
Pulsar::PolnSpectrumStats::get_stokes (unsigned ibin) const
{
  std::complex< Estimate<double> > zero ( 0.0, 0.0 );
  Stokes< std::complex< Estimate<double> > > result ( zero, zero, zero, zero );

  Stokes< Estimate<double> > re = real->get_stokes (ibin);
  Stokes< Estimate<double> > im = imag->get_stokes (ibin);

  for (unsigned ipol=0; ipol < 4; ipol++)
    result[ipol] = std::complex< Estimate<double> > (re[ipol], im[ipol]);

  return result;
}

//! Get the real componentStokes parameters for the specified harmonic
const Pulsar::PolnProfileStats*
Pulsar::PolnSpectrumStats::get_real () const
{
  return real;
}

//! Get the imaginary componentStokes parameters for the specified harmonic
const Pulsar::PolnProfileStats*
Pulsar::PolnSpectrumStats::get_imag () const
{
  return imag;
}

//! Returns the total determinant of the on-pulse phase bins
Estimate<double> Pulsar::PolnSpectrumStats::get_total_determinant () const
{
  return real->get_total_determinant() + imag->get_total_determinant();
}

//! Returns the variance of the baseline for the specified polarization
std::complex< Estimate<double> >
Pulsar::PolnSpectrumStats::get_baseline_variance (unsigned ipol) const
{
  return std::complex< Estimate<double> >
    ( real->get_baseline_variance(ipol), imag->get_baseline_variance(ipol) );
}

Pulsar::PolnProfile* fourier_to_psd (const Pulsar::PolnProfile* fourier)
{
  Reference::To<Pulsar::PolnProfile> psd = fourier->clone();
  detect (psd);
  return psd.release();
}

void copy_pad (Pulsar::PhaseWeight& into, Pulsar::PhaseWeight& from, float remainder)
{
  const unsigned from_nbin = from.get_nbin();
  const unsigned into_nbin = into.get_nbin();

  unsigned ibin = 0;
  for (; ibin < from_nbin; ibin++)
    into[ibin] = from[ibin];

  for (; ibin < into_nbin; ibin++)
    into[ibin] = remainder;
}

void Pulsar::PolnSpectrumStats::build () try
{
  if (!profile)
    return;

  fourier = fourier_transform (profile, plan);

  // convert to Stokes parameters and drop the Nyquist bin
  fourier->convert_state (Signal::Stokes);
  fourier->resize( profile->get_nbin() );

  // form the power spectral density
  Reference::To<PolnProfile> psd = fourier_to_psd (fourier);

  // separate fourier into real and imaginary components
  Reference::To<PolnProfile> re = psd->clone();
  Reference::To<PolnProfile> im = psd->clone();

  unsigned npol = 4;
  unsigned nbin = psd -> get_nbin();

  for (unsigned ipol=0; ipol < npol; ipol++)
  {
    float* C_ptr = fourier->get_Profile(ipol)->get_amps();
    float* re_ptr = re->get_Profile(ipol)->get_amps();
    float* im_ptr = im->get_Profile(ipol)->get_amps();

    for (unsigned ibin=0; ibin < nbin; ibin++)
    {
      re_ptr[ibin] = C_ptr[ibin*2];
      im_ptr[ibin] = C_ptr[ibin*2+1];
    }
  }

  if (!regions_set)
  {
    LastHarmonic last;
    last.set_Profile( psd->get_Profile(0) );

    last.get_weight (&onpulse);
    last.get_baseline_estimator()->get_weight (&baseline);

    last_harmonic = last.get_last_harmonic();

#ifdef _DEBUG
    cerr << "Pulsar::PolnSpectrumStats::build last harmonic=" 
	 << last_harmonic << " nbin on=" << onpulse.get_weight_sum() << endl;
#endif

    real->set_regions (onpulse, baseline);
    imag->set_regions (onpulse, baseline);
  }

  if (onpulse.get_nbin () != re->get_nbin())
  {
    PhaseWeight on_temp = onpulse;
    PhaseWeight off_temp = baseline;

    on_temp.resize( re->get_nbin() );
    off_temp.resize( re->get_nbin() );

    if (re->get_nbin() > onpulse.get_nbin ())
    {
      copy_pad( on_temp, onpulse, 0 );
      copy_pad( off_temp, baseline, 1 );
    }

    real->set_regions (on_temp, off_temp);
    imag->set_regions (on_temp, off_temp);
  }

  real->set_profile (re);
  imag->set_profile (im);

}
catch (Error& error)
{
  throw error += "Pulsar::PolnSpectrumStats::build";
}

void Pulsar::PolnSpectrumStats::set_plan (FTransform::Plan* p)
{
  plan = p;
}
