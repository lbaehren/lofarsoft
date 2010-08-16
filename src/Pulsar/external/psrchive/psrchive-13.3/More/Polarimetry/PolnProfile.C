/***************************************************************************
 *
 *   Copyright (C) 2003 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/PolnProfile.h"
#include "Pulsar/Profile.h"

#include "Pulsar/StokesCovariance.h"
#include "Pulsar/FourthMoments.h"

#include "Pulsar/ProfileAmpsExpert.h"
#include "Pulsar/ExponentialBaseline.h"
#include "Pulsar/PhaseWeight.h"

#include "Pulsar/Fourier.h"
#include "FTransform.h"
#include "ModifyRestore.h"
#include "Pauli.h"
#include "Error.h"

#ifdef sun
#include <ieeefp.h>
#endif

using namespace std;

/*! When transforming Profile objects, the absolute gain of the
  transformation may artificially inflate the weight of the Profile
  and incorrectly skew mean results. */
bool Pulsar::PolnProfile::normalize_weight_by_absolute_gain = false;

Pulsar::PolnProfile::PolnProfile ()
{
  state = Signal::Stokes;
  basis = Signal::Linear;
}

Pulsar::PolnProfile* Pulsar::PolnProfile::clone () const
{
  return new PolnProfile (basis, state,
			  profile[0]->clone(), profile[1]->clone(),
			  profile[2]->clone(), profile[3]->clone());
}

//
//
//
Pulsar::PolnProfile::PolnProfile (Signal::Basis _basis, Signal::State _state, 
				  Profile* _p0, Profile* _p1,
				  Profile* _p2, Profile* _p3)
{
  basis = _basis;
  state = _state;

  profile[0] = _p0;
  profile[1] = _p1;
  profile[2] = _p2;
  profile[3] = _p3;

  unsigned nbin = _p0->get_nbin();

  for (unsigned ipol=1; ipol < 4; ipol++)
    if (profile[ipol]->get_nbin() != nbin)
      throw Error (InvalidParam, "Pulsar::PolnPofile::PolnProfile",
		   "ipol=%d unequal nbin=%d != nbin=%d", ipol,
		   profile[ipol]->get_nbin(), nbin);

  FourthMoments* fourth = profile[0]->get<FourthMoments>();
  if (fourth && fourth->get_size() == StokesCovariance::nmoment)
    covariance = new StokesCovariance (fourth);
}

//
//
//
Pulsar::PolnProfile::PolnProfile (unsigned nbin)
{
  state = Signal::Stokes;
  basis = Signal::Linear;

  for (unsigned ipol=0; ipol < 4; ipol++)
    profile[ipol] = new Profile (nbin);
}

//
//
//
Pulsar::PolnProfile::~PolnProfile ()
{
  if (Profile::verbose)
    cerr << "Pulsar::PolnProfile destructor" << endl;
}

//
//
//
void Pulsar::PolnProfile::resize (unsigned nbin)
{
  for (unsigned ipol=0; ipol < 4; ipol++)
  {
    if (!profile[ipol])
      profile[ipol] = new Profile;
    profile[ipol]->resize (nbin);
  }

  if (covariance)
    covariance->resize (nbin);
}

//
//
//
void Pulsar::PolnProfile::sum (const PolnProfile* that)
{
  for (unsigned ipol=0; ipol < 4; ipol++)
    this->profile[ipol]->sum( that->profile[ipol] );
}


//
//
//
void Pulsar::PolnProfile::diff (const PolnProfile* that)
{
  for (unsigned ipol=0; ipol < 4; ipol++)
    this->profile[ipol]->diff( that->profile[ipol] );
}




//
//
//
unsigned Pulsar::PolnProfile::get_nbin () const
{
  return profile[0]->get_nbin();
}

//
//
//
const Pulsar::Profile* Pulsar::PolnProfile::get_Profile (unsigned ipol) const
{
  if (ipol >= 4)
    throw Error (InvalidRange, "PolnProfile::get_Profile",
		 "ipol=%d >= npol=4", ipol);

  return profile[ipol];
}

//
//
//
Pulsar::Profile* Pulsar::PolnProfile::get_Profile (unsigned ipol)
{
  if (ipol >= 4)
    throw Error (InvalidRange, "PolnProfile::get_Profile",
		 "ipol=%d >= npol=4", ipol);

  return profile[ipol];
}

//
//
//
const float* Pulsar::PolnProfile::get_amps (unsigned ipol) const
{
  if (ipol >= 4)
    throw Error (InvalidRange, "PolnProfile::get_amps",
		 "ipol=%d >= npol=4", ipol);

  return profile[ipol]->get_amps();
}

//
//
//
float* Pulsar::PolnProfile::get_amps (unsigned ipol)
{
  if (ipol >= 4)
    throw Error (InvalidRange, "PolnProfile::get_amps",
		 "ipol=%d >= npol=4", ipol);

  return profile[ipol]->get_amps();
}

//
//
//
void Pulsar::PolnProfile::set_amps (unsigned ipol, float* amps)
{
  if (ipol >= 4)
    throw Error (InvalidRange, "PolnProfile::set_amps",
                 "ipol=%d >= npol=4", ipol);

  profile[ipol]->set_amps(amps);
}

//
//
//
Pulsar::PhaseWeight* Pulsar::PolnProfile::get_baseline () const
{
  if (baseline)
    return baseline;
  
  if (!my_baseline)
    my_baseline = profile[0]->baseline();

  return my_baseline;
}

const Pulsar::StokesCovariance* Pulsar::PolnProfile::get_covariance () const
{
  return covariance;
}


void Pulsar::PolnProfile::check (const char* method,
				 Signal::State want_state,
				 unsigned want_ibin) const
{
  if (state != want_state)
    throw Error (InvalidRange, "PolnProfile::" + string(method),
		 "state=" + Signal::State2string(state) + " != " 
		 "want=" + Signal::State2string(state));

  if (want_ibin >= get_nbin())
    throw Error (InvalidRange, "PolnProfile::" + string(method),
		 "ibin=%d >= nbin=%d", want_ibin, get_nbin());
}

//
//
//
Stokes<float> Pulsar::PolnProfile::get_Stokes (unsigned ibin) const
{
  check ("get_Stokes", Signal::Stokes, ibin);

  return Stokes<float> (profile[0]->get_amps()[ibin],
			profile[1]->get_amps()[ibin],
			profile[2]->get_amps()[ibin],
			profile[3]->get_amps()[ibin]);
}

//
//
//
void Pulsar::PolnProfile::set_Stokes (unsigned ibin, 
				      const Stokes<float>& new_amps)
{
  check ("set_Stokes", Signal::Stokes, ibin);

  profile[0]->get_amps()[ibin] = new_amps[0];
  profile[1]->get_amps()[ibin] = new_amps[1];
  profile[2]->get_amps()[ibin] = new_amps[2];
  profile[3]->get_amps()[ibin] = new_amps[3];
}

//
//
//
Quaternion<float,Hermitian>
Pulsar::PolnProfile::get_pseudoStokes (unsigned ibin) const
{
  check ("get_pseudoStokes", Signal::PseudoStokes, ibin);

  return Quaternion<float,Hermitian> (profile[0]->get_amps()[ibin],
				      profile[1]->get_amps()[ibin],
				      profile[2]->get_amps()[ibin],
				      profile[3]->get_amps()[ibin]);
}

//
//
//
void 
Pulsar::PolnProfile::set_pseudoStokes (unsigned ibin, 
				       const Quaternion<float,Hermitian>& S)
{
  check ("set_pseudoStokes", Signal::PseudoStokes, ibin);

  profile[0]->get_amps()[ibin] = S[0];
  profile[1]->get_amps()[ibin] = S[1];
  profile[2]->get_amps()[ibin] = S[2];
  profile[3]->get_amps()[ibin] = S[3];
}

//
//
//
Jones<double> Pulsar::PolnProfile::get_coherence (unsigned ibin) const
{
  check ("get_coherence", Signal::Coherence, ibin);

  complex<double> cross (profile[2]->get_amps()[ibin], 
                         profile[3]->get_amps()[ibin]);

  return Jones<double> (profile[0]->get_amps()[ibin], conj(cross),
                        cross, profile[1]->get_amps()[ibin]);
}

//
//
//
void Pulsar::PolnProfile::set_coherence (unsigned ibin,
                                         const Jones<double>& new_amps)
{
  check ("set_coherence", Signal::Coherence, ibin);

  profile[0]->get_amps()[ibin] = new_amps(0,0).real();
  profile[1]->get_amps()[ibin] = new_amps(1,1).real();
  profile[2]->get_amps()[ibin] = new_amps(0,1).real();
  profile[3]->get_amps()[ibin] = new_amps(1,0).imag();

}

//! Set the baseline used by some methods
void Pulsar::PolnProfile::set_baseline (PhaseWeight* mask)
{
  baseline = mask;
}

void Pulsar::PolnProfile::scale (double scale)
{
  for (unsigned ipol=0; ipol < 4; ipol++)
    profile[ipol]->scale (scale);

  if (covariance)
    covariance->scale (scale);
}

void Pulsar::PolnProfile::rotate_phase (double phase)
{
  for (unsigned ipol=0; ipol < 4; ipol++)
    profile[ipol]->rotate_phase (phase);

  if (covariance)
    covariance->rotate_phase (phase);
}

template<typename T, typename U>
const Quaternion<T,Hermitian> transform (const Quaternion<T,Hermitian>& input,
					 const Jones<U>& jones)
{
  return real( convert (jones * convert(input) * herm(jones)) );
}

//
//
//
void Pulsar::PolnProfile::transform (const Jones<double>& response)
{
  if (Profile::verbose)
    cerr << "Pulsar::PolnProfile::transform response=" << response << endl;

  unsigned nbin = get_Profile(0)->get_nbin();

  float Gain = abs( det(response) );
  if (!finite(Gain))
    throw Error (InvalidParam, "Pulsar::PolnProfile::transform",
                 "non-invertbile response.  det(J)=%f", Gain);

  if (Gain == 0)
  {
    if (Profile::verbose)
      cerr << "Pulsar::PolnProfile::transform zero response" << endl;

    for (unsigned ipol=0; ipol < 4; ipol++)
      get_Profile(ipol)->set_weight ( 0.0 );

    return;
  }

  if (state == Signal::Stokes)
  {
    for (unsigned ibin = 0; ibin < nbin; ibin++)
      set_Stokes (ibin, ::transform (get_Stokes(ibin), response));

    if (covariance)
      covariance->transform (response);
  }  
  else if (state == Signal::Coherence)
  {
    Jones<float> response_dagger = herm(response);
    for (unsigned ibin = 0; ibin < nbin; ibin++)
      set_coherence (ibin, (response * get_coherence(ibin)) * response_dagger);
  }
  else if (state == Signal::PseudoStokes)
  {
    for (unsigned ibin = 0; ibin < nbin; ibin++)
      set_pseudoStokes (ibin, ::transform (get_pseudoStokes(ibin), response));
  }
  else
    throw Error (InvalidState, "Pulsar::PolnProfile::transform",
		 "unknown state=" + Signal::State2string(state));

  if (normalize_weight_by_absolute_gain)
    for (unsigned ipol=0; ipol < 4; ipol++)
      get_Profile(ipol)->set_weight ( get_Profile(ipol)->get_weight() / Gain );
}

//
//
//
void Pulsar::PolnProfile::transform (const Matrix<4,4,double>& response)
{
  if (Profile::verbose)
    cerr << "Pulsar::PolnProfile::transform response=\n" << response << endl;

  unsigned nbin = get_Profile(0)->get_nbin();

  for (unsigned ibin = 0; ibin < nbin; ibin++)
    set_Stokes (ibin, response * get_Stokes(ibin));

  if (covariance)
    covariance->transform (response);
}

//
//
//
double Pulsar::PolnProfile::sum (int bin_start, int bin_end) const
{
  double sum = 0;
  for (unsigned ipol=0; ipol < 4; ipol++)
    sum += profile[ipol]->sum( bin_start, bin_end );
  return sum;
}

//
//
//
double Pulsar::PolnProfile::sumsq (int bin_start, int bin_end) const
{
  double sumsq = 0;
  for (unsigned ipol=0; ipol < 4; ipol++)
    sumsq += profile[ipol]->sumsq( bin_start, bin_end );
  return sumsq;
}

//
//
//
void Pulsar::PolnProfile::convert_state (Signal::State out_state)
{
  if (out_state == state)
    return;

  if (out_state == Signal::Stokes)
  {
    if (state == Signal::Coherence)
    {
      sum_difference (profile[0], profile[1]);
    
      // data 2 and 3 are equivalent to 2*Re[PQ] and 2*Im[PQ]
      *(profile[2]) *= 2.0;
      *(profile[3]) *= 2.0;

      state = Signal::PseudoStokes;
    }

    if (basis == Signal::Circular)
    {
      float* V = profile[1]->get_amps();
      float* Q = profile[2]->get_amps();
      float* U = profile[3]->get_amps();

      ProfileAmps::Expert::set_amps_ptr( profile[1], Q );
      ProfileAmps::Expert::set_amps_ptr( profile[2], U );
      ProfileAmps::Expert::set_amps_ptr( profile[3], V );
    }

    // record the new state
    state = Signal::Stokes;
  }
  else if (out_state == Signal::Coherence)
  {
    // cerr << "convert_state to Signal::Coherence" << endl;

    if (state == Signal::Stokes && basis == Signal::Circular)
    {
      float* ReLR   = profile[1]->get_amps();
      float* ImLR   = profile[2]->get_amps();
      float* diffLR = profile[3]->get_amps();

      ProfileAmps::Expert::set_amps_ptr( profile[1], diffLR );
      ProfileAmps::Expert::set_amps_ptr( profile[2], ReLR );
      ProfileAmps::Expert::set_amps_ptr( profile[3], ImLR );

      state = Signal::PseudoStokes;
    }

    sum_difference (profile[0], profile[1]);

    // The above sum and difference produced 2*PP and 2*QQ.  As well,
    // data 2 and 3 are equivalent to 2*Re[PQ] and 2*Im[PQ].
    *(profile[0]) *= 0.5;
    *(profile[1]) *= 0.5;
    *(profile[2]) *= 0.5;
    *(profile[3]) *= 0.5;

    // record the new state
    state = Signal::Coherence;

  }
  else
    throw Error (InvalidParam, "Pulsar::PolnProfile::convert_state",
		 "invalid output state %s", Signal::state_string (out_state));
}

/*! \retval sum = sum + difference
    \retval difference = sum - difference
*/
void Pulsar::PolnProfile::sum_difference (Profile* sum, Profile* difference)
{
  unsigned nbin = sum->get_nbin();

  if (nbin != difference->get_nbin())
    throw Error (InvalidParam, "Pulsar::PolnProfile::sum_difference",
		 "nbin=%d != nbin=%d", nbin, difference->get_nbin());

  float* s = sum->get_amps ();
  float* d = difference->get_amps ();
  float temp;

  for (unsigned ibin=0; ibin<nbin; ibin++) {
    temp = s[ibin];
    s[ibin] += d[ibin];
    d[ibin] = temp - d[ibin];
  }
}


/*!
  Currently, this method should be used only to correct Stokes parameters
  that have been formed assuming linearly polarized receptors when in
  fact the receptors are circularly polarized.
*/
void Pulsar::PolnProfile::convert_basis (Signal::Basis to)
{
  if (state == Signal::Stokes && to == Signal::Circular)
  {
    cout << "Converting to Circular" << endl;
    float* V = profile[1]->get_amps();
    float* Q = profile[2]->get_amps();
    float* U = profile[3]->get_amps();
    
    ProfileAmps::Expert::set_amps_ptr( profile[1], Q );
    ProfileAmps::Expert::set_amps_ptr( profile[2], U );
    ProfileAmps::Expert::set_amps_ptr( profile[3], V );

    basis = to;
  }
}
				      

/*! 
  Forms the Stokes polarimetric invariant interval,
  \f$\sqrt{I^2-Q^2-U^2-V^2}\f$, for every phase bin so that,
  upon completion, npol == 1 and state == Signal::Invariant.

  \pre The profile baselines must have been removed (unchecked).
  \post The bias due to noise is removed
*/
void Pulsar::PolnProfile::invint (Profile* invint, bool second) const
{
  unsigned nbin = get_nbin();
  invint->resize (nbin);

  if (state == Signal::Stokes || state == Signal::PseudoStokes)
    for (unsigned ibin = 0; ibin < nbin; ibin++)
      invint->get_amps()[ibin] = get_Stokes(ibin).invariant ();
  
  else if (state == Signal::Coherence)
    for (unsigned ibin = 0; ibin < nbin; ibin++)
      invint->get_amps()[ibin] = 4.0 * abs( det(get_coherence(ibin)) );

  /*
    derive the baseline from the total intensity profile because its
    statistics are better modelled
  */
  Reference::To<Pulsar::PhaseWeight> use_baseline = get_baseline ();

  /*
    remove the bias due to noise from the invariant profile
  */
  use_baseline->set_Profile( invint );
  invint->offset( -use_baseline->get_mean().get_value() );

  // return to a second-order moment
  if (second)
    invint->square_root();

  invint->set_centre_frequency ( get_Profile(0)->get_centre_frequency() );
  invint->set_weight ( get_Profile(0)->get_weight() );
}

void Pulsar::PolnProfile::invconv (Profile* invconv) const
{
  unsigned nbin = get_nbin();

  Reference::To<PolnProfile> fourier = complex_fourier_transform (this);

  Signal::State state = get_state();

  Jones<double> coherence;
  complex<double> Ci (0,1);

  vector<float> fourier_det (nbin * 2);

  for (unsigned ibin = 0; ibin < nbin; ibin++)
  {
    if (state == Signal::Stokes || state == Signal::PseudoStokes)
    {
      Stokes< complex<double> > real = fourier->get_Stokes(ibin*2);
      Stokes< complex<double> > imag = fourier->get_Stokes(ibin*2+1);

      Stokes< complex<double> > stokes = real + Ci*imag;

      coherence = convert (stokes);
    }
    else if (state == Signal::Coherence)
    {
      Jones<double> real = fourier->get_coherence(ibin*2);
      Jones<double> imag = fourier->get_coherence(ibin*2+1);

      coherence = real + Ci*imag;
    }

    complex<double> determinant = det(coherence);
    fourier_det[ibin*2]   = determinant.real();
    fourier_det[ibin*2+1] = determinant.imag();
  }

  vector<float> complex_data (nbin * 2);

  FTransform::bcc1d (nbin, &(complex_data[0]), &(fourier_det[0]));

  invconv->resize (nbin);
  float* amps = invconv->get_amps();

  for (unsigned ibin = 0; ibin < nbin; ibin++)
    amps[ibin] = complex_data[ibin*2];

  invconv->set_centre_frequency ( get_Profile(0)->get_centre_frequency() );
  invconv->set_weight ( get_Profile(0)->get_weight() );
}


bool pav_backward_compatibility = false;

void remove_baseline (Pulsar::Profile* profile, 
		      Pulsar::BaselineEstimator* estimator)
{
  Reference::To<Pulsar::PhaseWeight> baseline;
  if (estimator)
    baseline = estimator->baseline( profile );
  else
    baseline = profile->baseline();
  profile->offset( -baseline->get_mean().get_value() );
}

/*!
  rss stands for root-sum-squared

  \pre The PolnProfile must contain Stokes parameters; so as to reduce the
  amount of behind the scenes cloning and state conversion

  \pre The Profile baselines should have been removed; so as not to
  interfere with any baseline removal algorithms already applied

  \post The polarization profiles from jpol to kpol inclusive are added

  \post The bias due to noise is removed before the square root is taken

 */
void Pulsar::PolnProfile::get_rss 
( Profile* rss, unsigned jpol, unsigned kpol,
  BaselineEstimator* baseline_estimator) const
{
  if (state != Signal::Stokes)
    throw Error (InvalidState, "Pulsar::PolnProfile::get_sqrt_sumsq",
		 "must first convert to Stokes parameters");

  unsigned ibin, nbin = get_nbin();
  
  rss->resize (nbin);
  rss->zero ();

  float* amps = rss->get_amps();

  for (unsigned ipol=jpol; ipol <= kpol; ipol++) {
    const float *a = get_Profile(ipol)->get_amps();
    for (ibin=0; ibin<nbin; ibin++)
      amps[ibin] += a[ibin]*a[ibin];
  }

  if (!pav_backward_compatibility)
    remove_baseline (rss, baseline_estimator);

  for (ibin=0; ibin<nbin; ibin++)
    if (amps[ibin] < 0.0)
      amps[ibin] = -sqrt(-amps[ibin]);
    else
      amps[ibin] = sqrt(amps[ibin]);

  if (pav_backward_compatibility)
    remove_baseline (rss, baseline_estimator);

}

void Pulsar::PolnProfile::get_polarized (Profile* polarized) const
try {
  get_rss (polarized, 1,3);
}
catch (Error& error) {
  throw error += "Pulsar::PolnProfile::get_polarized";
}

void Pulsar::PolnProfile::get_linear (Profile* linear) const
try {

  ExponentialBaseline estimator;
  get_rss (linear, 1,2, &estimator);

}
catch (Error& error) {
  throw error += "Pulsar::PolnProfile::get_linear";
}

void Pulsar::PolnProfile::get_circular (Profile* absV) const try
{
  absV->operator=( *get_Profile(3) );
  absV->absolute();
}
catch (Error& error)
{
  throw error += "Pulsar::PolnProfile::get_circular";
}

void Pulsar::PolnProfile::get_ellipticity (vector< Estimate<double> >& ell,
					   float threshold) const
{
  if (state != Signal::Stokes)
    throw Error (InvalidState, "Pulsar::PolnProfile::get_PA",
		 "must first convert to Stokes parameters");
   
  Profile polarized;
  get_polarized (&polarized);

  Reference::To<PhaseWeight> base = get_baseline();

  // Apply I baseline to determine Q, U variance.
  base->set_Profile(get_Profile(1));
  double var_q = base->get_variance().get_value();
  base->set_Profile(get_Profile(2));
  double var_u = base->get_variance().get_value();
  base->set_Profile(get_Profile(3));
  double var_v = base->get_variance().get_value();

  float sigma = sqrt (0.5*(var_q + var_u + var_v));

  Profile linear;
  get_linear (&linear);
  double var_l = 0.5 * (var_q + var_u);

  const float *l = linear.get_amps();
  const float *v = get_Profile(3)->get_amps(); 

  unsigned nbin = get_nbin();
  ell.resize (nbin);

  for (unsigned ibin=0; ibin<nbin; ibin++)
  {
    if (!threshold || polarized.get_amps()[ibin] > threshold*sigma)
    {
      Estimate<double> L (l[ibin], var_l);
      Estimate<double> V (v[ibin], var_v);
      ell[ibin] = 90.0/M_PI * atan2 (V, L);
    }
    else
      ell[ibin] = 0.0;
  }

}

void Pulsar::PolnProfile::get_linear (vector< complex< Estimate<double> > >& L,
				      float threshold) const
{
  Profile linear;
  get_linear (&linear);

  // Determine baseline weights from I profile.
  Reference::To<PhaseWeight> base = get_baseline();
  double sigma = base->get_variance().get_value();
  sigma = sqrt(sigma);

  // Apply I baseline to determine Q, U variance.
  base->set_Profile(get_Profile(1));
  double var_q = base->get_variance().get_value();
  base->set_Profile(get_Profile(2));
  double var_u = base->get_variance().get_value();

  const float *q = get_Profile(1)->get_amps(); 
  const float *u = get_Profile(2)->get_amps(); 

  unsigned nbin = get_nbin();
  L.resize (nbin);

  for (unsigned ibin=0; ibin<nbin; ibin++)
  {
    if (!threshold || linear.get_amps()[ibin] > threshold*sigma)
    {
      Estimate<double> Q (q[ibin], var_q);
      Estimate<double> U (u[ibin], var_u);

      L[ibin] = complex< Estimate<double> > (Q, U);
    }
    else
      L[ibin] = 0.0;
  }

}

void Pulsar::PolnProfile::get_orientation (vector< Estimate<double> >& posang,
					   float threshold) const
{
  vector< complex< Estimate<double> > > linear;
  get_linear (linear, threshold);

  unsigned nbin = get_nbin();
  posang.resize (nbin);

  for (unsigned ibin=0; ibin<nbin; ibin++)
  {
    if (linear[ibin].real().get_variance() != 0)
    {
      Estimate<double> Q = linear[ibin].real();
      Estimate<double> U = linear[ibin].imag();
      posang[ibin] = 90.0/M_PI * atan2 (U,Q);
    }
    else
      posang[ibin] = 0;
  }
}
