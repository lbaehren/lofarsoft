/***************************************************************************
 *
 *   Copyright (C) 2002 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
using namespace std;
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <iostream>

#include <assert.h>
#include <math.h>

#include "FTransform.h"
#include "model_profile.h"

using namespace std;

#define SQR(x) ((x)*(x))

#define F77_fccf F77_FUNC(fccf,FCCF)

extern "C" {
  void F77_fccf (float *, float *, float *);
}

int Pulsar::max_harmonic = 0;

int Pulsar::model_profile (int npts, int narrays, 
                           const float* const * prf, const float* const* std,
		           double* scale, double* sigma_scale, 
		           double* shift, double* sigma_shift,
		           double* chisq, int verbose)
{
  float ** fft_std = new float*[narrays];
  float ** fft_prf = new float*[narrays];
  float ** xcorr_amps = new float*[narrays];
  float ** xcorr_phases = new float*[narrays];

  assert (fft_std!=0 && fft_prf!=0 && 
	  xcorr_amps!=0 && xcorr_phases!=0);

  int npt2 = npts/2;

  if (max_harmonic)
    npt2 = max_harmonic;

  int i,j;

  for (i=0; i<narrays; ++i) {

    fft_std[i] = new float[npts+2];
    fft_prf[i] = new float[npts+2];
    xcorr_amps[i] = new float[npt2];
    xcorr_phases[i] = new float[npt2];

    assert (fft_std[i]!=0 && fft_prf[i]!=0 && 
	    xcorr_amps[i]!=0 && xcorr_phases[i]!=0);

    FTransform::frc1d (npts, fft_std[i], std[i]);
    FTransform::frc1d (npts, fft_prf[i], prf[i]);

    // zap the Nyquist value
    fft_std[i][1] = fft_prf[i][1] = 0.0;

    for (j=0; j<npt2; ++j) {

      xcorr_amps[i][j] = sqrt( SQR(fft_std[i][2*j]) + SQR(fft_std[i][2*j+1]) )
	* sqrt( SQR(fft_prf[i][2*j]) + SQR(fft_prf[i][2*j+1]) );
      
      xcorr_phases[i][j] = -atan2(fft_prf[i][2*j+1], fft_prf[i][2*j]) +
	atan2(fft_std[i][2*j+1], fft_std[i][2*j]);

    }
  }

  // Compute an initial estimation of the shift based on the
  // (discrete) cross-correlation function

  float xcorr_shift;
  F77_fccf (&(xcorr_amps[0][1]), &(xcorr_phases[0][1]), &xcorr_shift);

  if (verbose)
    cerr << "xcorr_shift=" << xcorr_shift << endl;

  // Run through successively larger numbers of frequency components
  // finding the best-fitting shift between the two profiles. We
  // start at 32 frequency components and continue up until npts/2
  // components.
  //
  // N.B. we do not include the DC component of the fourier transform
  // in our sums.

  double tau = (double) xcorr_shift;
  double dtau = 0, edtau = 0;
  double deriv_chisq = 0;
  double low_tau = 0, low_deriv_chisq = 0, high_tau = 0, high_deriv_chisq = 0;
  int start_bin = 32;

  // Allow this loop index to go all the way up to npts/2, the second
  // loop will ensure the that the old Nyquist index is not accessed

  for (int nsum=start_bin; nsum<=npt2; nsum*=2) {
    dtau = 2*M_PI/(float)(5.0*nsum);
    edtau = 1.0/(float)(2.0*nsum+1.0);
    if(nsum>npts/4.0) edtau = .00001;

    int ntries = 0;
    int low = 0, high = 0;

    while (low == 0 || high == 0)
    {
      ntries++;
      if (ntries > 100)
      {
	cerr << "model_profile: max retries (100) exceeded" << endl;
        return(-1);
      }

      deriv_chisq = 0;
      for(i=0; i<narrays; ++i)
	for(int iter=1; iter<nsum; ++iter)
	  deriv_chisq+=iter*xcorr_amps[i][iter]*sin(-xcorr_phases[i][iter]+iter*tau);

      if (deriv_chisq<0)
      {
        low_tau = tau;
        low_deriv_chisq = deriv_chisq;
        tau += dtau;
        low = 1;
      }
      else
      {
        high_tau = tau;
        high_deriv_chisq = deriv_chisq;
        tau -= dtau;
        high = 1;
      }

    }
    tau = zbrent (low_tau, high_tau, low_deriv_chisq, high_deriv_chisq, 
		  edtau, narrays, xcorr_amps, xcorr_phases, nsum);
  }

  if (verbose) 
    cerr << "model profile: best tau is " << tau << endl;

  // These relationships are discussed in Joe Taylor's paper, "Pulsar
  // Timing and Relativistic Gravity", Philosophical Transactions:
  // Physical Sciences and Engineering, Vol. 341, No. 1660, "Pulsars
  // as Physics Laboratories" (Oct 15, 1992), pages 117-134.

  // The profile is related to the standard by:
  //
  // P(t) = a + scale*S(t-tau) + N(t)
  //
  // where:
  //   P = test profile 
  //   S = standard template
  //   N = noise 
  // tau = shift between profile and template

  double s1=0, s2=0, s3=0;
  double cosfac;
  for(i=0; i<narrays; ++i){
    for(int j=1; j<npt2; ++j){
      cosfac = cos(-xcorr_phases[i][j]+j*tau);
      s1 += xcorr_amps[i][j]*cosfac;
      s2 += SQR(fft_std[i][2*j]) + SQR(fft_std[i][2*j+1]);
      s3 += j*j*xcorr_amps[i][j]*cosfac;
    }
  }

  if (s1<=0 || s2==0 || s3<=0)
  {
    cerr << "model_profile: aborting before floating point exception" << endl;
    if (s1<=0)
      cerr << "  Numerator to Equation A9 = " << s1 << endl;
    if (s2==0)
      cerr << "  Denominator to Equations A9 and A11 equals zero" << endl;
    if (s3<=0)
      cerr << "  Denominator to Equation A10 = " << s3 << endl;
    return -1;
  }

  *scale = s1/s2;
  *chisq = 0;
  float fft_prf_amp, fft_std_amp;
  for(i=0; i<narrays; ++i){
    for(int j=1; j<npt2; ++j){
      fft_prf_amp = sqrt( SQR(fft_prf[i][2*j]) + SQR(fft_prf[i][2*j+1]) );
      fft_std_amp = sqrt( SQR(fft_std[i][2*j]) + SQR(fft_std[i][2*j+1]) );
      *chisq += fft_prf_amp*fft_prf_amp - 
	2**scale*fft_prf_amp*fft_std_amp*cos(xcorr_phases[i][j]-j*tau)+
	*scale**scale*fft_std_amp*fft_std_amp;
    }
  }

  if (*chisq <= 0)
  {
    cerr << "model_profile: chisq <= 0" << endl;
    return -1;
  }

  // This defines the errors in scale and shift so that the reduced
  // chisq is unity.  We subtract 1 D.O.F. for tau.

  double rms = sqrt( *chisq / (float(narrays*(npt2-1)) - 1.0) );
  double fac = npts/(2.0*M_PI);
  *sigma_scale = rms/sqrt(2.0*s2);
  *shift = fac*tau;
  *sigma_shift = fac * rms / sqrt(2.0* *scale * s3);

  for (i=0; i<narrays; ++i) {
    delete [] fft_std[i];
    delete [] fft_prf[i];
    delete [] xcorr_amps[i];
    delete [] xcorr_phases[i];
  }

  delete [] fft_std;
  delete [] fft_prf;
  delete [] xcorr_amps;
  delete [] xcorr_phases;

  return 0;
}

double Pulsar::zbrent(float low_tau, float high_tau,
	              float low_deriv_chisq, float high_deriv_chisq, 
	              float edtau, int narrays, 
	              float ** xcorr_amps, float ** xcorr_phases, int nsum)
{
  int i_max = 100;
  double eps = .00000006;
  double a = low_tau;
  double b = high_tau;
  double c = high_tau; 
  double d = 0;
  double e = 0;
  double fa = low_deriv_chisq; 
  double fb = high_deriv_chisq;
  double fc = fb;


  for (int i=1; i<i_max; ++i) {
    if(fb*fc>0){
      c  = a;
      fc = fa;
      d  = b-a;
      e  = d;
    }
    if(fabs(fc)<fabs(fb)){
      a  = b;
      b  = c;
      c  = a;
      fa = fb;
      fb = fc;
      fc = fa;
    }
    double tol1 = 2*eps *fabs(b)+.5*edtau;
    double xm = .5*(c-b);
    if(fabs(xm)<=tol1 || fb == 0) return(b);
    if(fabs(e)>=tol1 && fabs(fa)>fabs(fb)){
      double p, q, r;
      double s = fb/fa;
      if(a==c){
	p = 2*xm*s;
	q = 1-s;
      } else {
	q = fa/fc;
	r = fb/fc;
	p = s*(2*xm*q*(q-r)-(b-a)*(r-1));
	q = (q-1)*(r-1)*(s-1);
      }
      if(p>0) q = -q;
      p = fabs(p);
      if(2*p<(3*xm*q-fabs(tol1*q)) && 2*p<fabs(e*q)){
	e = d;
	d = p/q;
      } else {
	d = xm;
	e = d;
      }
    } else {
      d = xm;
      e = d;
    }
    a = b;
    fa = fb;
    if(fabs(d)>tol1) b+=d;
    else b+=tol1*xm/fabs(xm);
    fb = 0;
    for(int j=0; j<narrays; ++j) {
      for(int iter=1; iter<nsum; ++iter)
	fb+=iter*xcorr_amps[j][iter]*sin(-xcorr_phases[j][iter]+iter*b); }
  }
  return(b);
}
