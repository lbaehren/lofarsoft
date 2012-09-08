/***************************************************************************
 *
 *   Copyright (C) 2010 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/BoostShiftAnalysis.h"
#include "Pulsar/PolnProfile.h"
#include "Pulsar/Fourier.h"

#include "Pulsar/ExponentialBaseline.h"
#include "Pulsar/PhaseWeight.h"
#include "Pulsar/LastSignificant.h"

using namespace std;

bool Pulsar::BoostShiftAnalysis::verbose = false;

void Pulsar::BoostShiftAnalysis::set_profile (const PolnProfile* profile)
{
  if (profile->get_state() != Signal::Stokes)
    throw Error (InvalidParam, "Pulsar::BoostShiftAnalysis::set_profile",
		 "polarization profile is not in Stokes state");

  fourier = fourier_transform (profile);

  Pulsar::LastSignificant last_significant;

  const unsigned npol = 4;

  for (unsigned ipol=0; ipol < npol; ipol++)
  {
    Profile* prof = fourier->get_Profile(0)->clone();
    detect (prof);

    ExponentialBaseline baseline;
    Reference::To<PhaseWeight> weight = baseline.baseline( prof );

    // the standard deviation of an exponential distribution equals its mean
    double rms = weight->get_mean().get_value();
    
    last_significant.find (prof, rms);
  }

  max_harmonic = last_significant.get();

  if (verbose)
    cerr << "Pulsar::BoostShiftAnalysis::set_profile last harmonic ibin="
	 << max_harmonic << endl;
}


/*! Returns the partial derivative of the m^th total intensity
  cross-spectral harmonic with respect to the k^th boost vector
  component, evaluated at \f$ \beta = 0 \f$.

  See Equation (24) of 1 November 2010 companion to van Straten (2006). */

std::complex<double> 
Pulsar::BoostShiftAnalysis::delS_delb (unsigned m, unsigned k) const
{
  float* S0 = fourier->get_Profile(0)->get_amps();
  float* Sk = fourier->get_Profile(k)->get_amps();

  std::complex<double> S0m (S0[m*2], S0[m*2+1]);
  std::complex<double> Skm (Sk[m*2], Sk[m*2+1]);
  Skm *= 2.0;

  return Skm * conj(S0m);
}


/*! Returns the squared modulus of the m^th total intensity
  cross-spectral harmonic. */

double Pulsar::BoostShiftAnalysis::S0sq (unsigned m) const
{
  float* S0 = fourier->get_Profile(0)->get_amps();

  std::complex<double> S0m (S0[m*2], S0[m*2+1]);

  return norm (S0m);
}

double Pulsar::BoostShiftAnalysis::delvarphi_delb (unsigned k) const
{
  double numerator = 0.0;
  double denominator = 0.0;

  for (unsigned m=1; m<max_harmonic; m++)
  {
    std::complex<double> delZm_delbk = delS_delb (m, k);
    numerator += delZm_delbk.imag() * m;

    double S0m_sq = S0sq (m);
    denominator += m * S0m_sq * m;
  }

  return numerator / (2*M_PI*denominator);
}

Vector<3,double> Pulsar::BoostShiftAnalysis::dotvarphi () const
{
  Vector<3,double> result;
  for (unsigned k=0; k<3; k++)
    result[k] = delvarphi_delb (k+1);
  return result;
}

double Pulsar::BoostShiftAnalysis::delvarphi_delbeta () const
{
  double beta_sq = 0;
  for (unsigned k=1; k<4; k++)
  {
    double delb = delvarphi_delb (k);
    beta_sq += delb * delb;
  }
  return sqrt(beta_sq);
}
