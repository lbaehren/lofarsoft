/***************************************************************************
 *
 *   Copyright (C) 2008 by Paul Demorest
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/AdaptiveSmooth.h"
#include "Pulsar/Profile.h"
#include "FTransform.h"

#include <gsl/gsl_errno.h>
#include <gsl/gsl_sf_bessel.h>

#include <complex>
#include <math.h>

using namespace std;

Pulsar::AdaptiveSmooth::AdaptiveSmooth ()
{
  max_harm=0;
  hold=false;
  noise_band=0.3;
  method = Sinc_IC;
  gsl_set_error_handler_off();
}

Pulsar::AdaptiveSmooth::~AdaptiveSmooth ()
{

}

float Pulsar::AdaptiveSmooth::get_turns () const
{
  double sum=0.0;
  for (unsigned i=0; i<filter.size(); i++) { sum += filter[i]; }
  return 1.0/sum;
}

unsigned Pulsar::AdaptiveSmooth::get_max_harm () const
{
  return max_harm;
}

void Pulsar::AdaptiveSmooth::set_hold(bool h) { hold=h; }

bool Pulsar::AdaptiveSmooth::get_hold() const { return hold; }

void Pulsar::AdaptiveSmooth::set_method(Pulsar::AdaptiveSmooth::Method m) {
  method = m;
}

void Pulsar::AdaptiveSmooth::transform(Profile *p)
{

  // Make fft of profile
  float *fprof = new float[p->get_nbin() + 2];
  FTransform::frc1d(p->get_nbin(), fprof, p->get_amps());

  // If no current filter or hold is off, compute optimal filter
  if (filter.size()==0 || !hold) 
    compute(fprof, p->get_nbin());

  // Apply filter to profile
  unsigned ih;
  const int nh = p->get_nbin()/2 + 1;
  complex<float> *cprof = (complex<float> *)fprof;
  for (ih=0; (ih<nh) && (ih<filter.size()); ih++) 
    cprof[ih] *= filter[ih];
  for (ih=ih; ih<nh; ih++)
    cprof[ih] = 0.0;

  // Inverse transform back to profile
  FTransform::bcr1d(p->get_nbin(), p->get_amps(), fprof);
  if (FTransform::get_norm() == FTransform::unnormalized)
    p->scale(1.0 / (double)p->get_nbin());

  // Free temp space
  delete [] fprof;
}

void Pulsar::AdaptiveSmooth::compute(const float *fprof, int nbin)
{
  // Number of harmonics (including DC)
  const int nh = nbin/2 + 1;

  // Reset existing filter
  filter.resize(nh);
  for (unsigned i=0; i<nh; i++) 
    filter[i] = 0.0;

  // Compute profile power spectrum
  float *pspec = new float[nh];
  const complex<float> *cprof = (const complex<float> *)fprof;
  for (unsigned i=0; i<nh; i++) 
    pspec[i] = norm(cprof[i]);

  // Estimate noise level
  double sigma2=0.0;
  int count=0;
  for (unsigned i=(1.0 - noise_band)*nh; i<nh; i++) {
    sigma2 += pspec[i];
    count++;
  }
  sigma2 /= (double)count;

  // Use a specific implementation
  if (method==Wiener)
    compute_wiener(pspec, sigma2, nh);
  else if (method==Sinc_IC)
    compute_sinc_ic(pspec, sigma2, nh);
  else if (method==Sinc_MSE)
    compute_sinc_mse(pspec, sigma2, nh);
  else if (method==VonMises)
    compute_vonmises_lpf(pspec, sigma2, nh);
  else 
    throw Error(InvalidState, "Pulsar::AdaptiveSmooth::compute",
        "Invalid filter method selected");

  // Free up mem
  delete [] pspec;

}

void Pulsar::AdaptiveSmooth::compute_wiener(const float *pspec,
    double sigma2, int nh) {
  // Basic Wiener filter (ie, see NR sec 13.3), with a 2-sigma
  // cutoff to remove noisy points.
  filter[0] = 1.0;
  for (unsigned i=1; i<nh; i++) {
    filter[i] = (pspec[i] - sigma2) / pspec[i];
    if (filter[i]<0.5) filter[i]=0.0;
  }
}

void Pulsar::AdaptiveSmooth::compute_sinc_mse(const float *pspec,
    double sigma2, int nh) 
{
  // Just brute-force this minimization:
  // Criterion is minimum expected MS diff between real and noisy signal,
  // same as in Wiener filter.  But here our denoising filter is 
  // constrained to be a "brick wall" (sinc in time domain). We ignore
  // DC in making this.
  double mse, mse_min=0.0;
  int ih_min=0;

  // Get total signal power
  double ptot=0.0;
  for (unsigned i=1; i<nh; i++) 
    ptot += pspec[i];

  // Loop over harmonics, look for min
  for (unsigned i=1; i<nh; i++) {
    ptot -= pspec[i];
    mse = (double)i*sigma2 + ptot - (double)(nh-1-i)*sigma2;
    if ((i==1) || (mse<mse_min)) { 
      mse_min = mse;
      ih_min = i;
    }
  }

  // Keep always at least 2 harmonics.
  if (ih_min<2) { ih_min=2; }

  // Fill in filter coeffs
  for (unsigned i=0; i<=ih_min; i++) 
    filter[i] = 1.0;
  for (unsigned i=ih_min+1; i<nh; i++) 
    filter[i] = 0.0;

  // fill in etc
  max_harm = ih_min;
}

void Pulsar::AdaptiveSmooth::compute_lpf(const float *pspec,
    double sigma2, int nh, float (*filter_func)(float,float))
{
  // Same minimization as above, but uses a generic filter
  // shape that takes a freq and cutoff freq as 
  // arguments.
  double mse, mse_min=0.0;
  int ih_min=0;

  for (unsigned ih=1; ih<nh*(1.0-noise_band); ih++) {

    // Compute MSE using ih as cutoff freq.
    mse=0.0;
    for (unsigned i=1; i<nh; i++) {
      double g = filter_func((float)i, (float)ih);
      mse += ((1.0-g)*(1.0-g)*(pspec[i]-sigma2) + g*g*sigma2);
    }

    // Keep this one if mse is minimum
    if ((ih==1) || (mse<mse_min)) {
      ih_min = ih;
      mse_min = mse;
    }

  }

  // Fill in filter coeffs
  for (unsigned ih=0; ih<nh; ih++) 
    filter[ih] = filter_func((float)ih, (float)ih_min);

}

/* Return freq-domain representation of von mises smoothing kernel */
static float von_mises_filter(float freq, float freq_cutoff) {
  const double k = freq_cutoff*freq_cutoff;
  const int n = (int)freq;
  gsl_sf_result top,bot;
  int rv = gsl_sf_bessel_In_scaled_e(n,k,&top);
  if (rv!=GSL_SUCCESS) return 0.0;
  rv = gsl_sf_bessel_I0_scaled_e(k,&bot);
  if (rv!=GSL_SUCCESS) return 0.0;
  return top.val/bot.val;
}

void Pulsar::AdaptiveSmooth::compute_vonmises_lpf(const float *pspec, 
    double sigma2, int nh) {
  compute_lpf(pspec, sigma2, nh, von_mises_filter);
}

void Pulsar::AdaptiveSmooth::compute_sinc_ic(const float *pspec,
    double sigma2, int nh) {

  // Compute optimal number of harmonics using various 
  // information criteria.  These seem to be a little
  // more conservative than the pure MSE criterion above.

  double rss=0.0;
  for (int i=1; i<nh; i++) { rss += pspec[i]; }

  double ic, ic_min;
  double ih_min;
  for (int i=1; i<nh; i++) {
    rss -= pspec[i];
    double n = (double)(nh-1-i);
    double k = (double)i;
    ic = n*log(rss/n) + k*log(n);          // This is BIC
    //ic = n*(log(2.0*M_PI*rss/n) + 1) + 2k; // This is AIC
    //ic = log(rss/n) + (n+k)/(n-k-2);       // This is AICc
    //ic = log(rss/(n-k)) + (n+k)/(n-k-2);   // This is AICu
    if (ic<ic_min || i==1) {
      ic_min = ic;
      ih_min = i;
    }
  }

  // Keep always at least 2 harmonics.
  if (ih_min<2) { ih_min=2; }

  // Fill in filter coeffs
  for (unsigned i=0; i<=ih_min; i++) 
    filter[i] = 1.0;
  for (unsigned i=ih_min+1; i<nh; i++) 
    filter[i] = 0.0;

  // fill in etc
  max_harm = ih_min;
}
