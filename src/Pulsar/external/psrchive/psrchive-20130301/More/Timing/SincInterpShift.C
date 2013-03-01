/***************************************************************************
 *
 *   Copyright (C) 2005 by Russell Edwards
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/SincInterpShift.h"
#include "Pulsar/Profile.h"
#include "FTransform.h"

#include <stdio.h>
#include <algorithm>
#include <math.h>

using namespace std;

// redwards --- code for finding the phase shift w.r.t. a template profile,
// using sinc interpolation of the cross correlation function

Pulsar::SincInterpShift::SincInterpShift ()
{
  zap_period = 0;
}

static float
sinc_interp(float *f, double x, int n)
{
  while (x < 0.0)
    x+= n;
  while (x >= n)
    x-= n;

  if (x == floor(x))
    return f[(int)floor(x)];
 
  // uses technique of Schanze, IEEE Trans Sig proc 42(6) 1502-1503 1995
  // for even n,
  //    f(x) = sin(pi.x)/pi sum_{i=-L}^{M-1} f[i] -1^i cot(pi.(x-i)/n)
  // where L+M=N.
  // We can (apparently?) choose L=0 to have M=N

  if (n%2)
  {
    fprintf(stderr, "Error, odd numbers of bins not implemented in sinc_interp!!\n"); // possible but uglier
    exit(1);
  }

  int i;
  double result = 0.0;
  for (i=0; i < n; i++)
  {
    result += f[i] * 1.0/tan(M_PI * (x-i)/n);
    i++;
    result -= f[i] * 1.0/tan(M_PI * (x-i)/n);
  }

//  return result * sin(M_PI*x)/n;  breaks for near-integer x
  return result * sin(M_PI*(x - 2.0*floor(0.5*x)))/n;
}

static float
sinc_interp_second_derivative(float *f, double x, int n)
{
  while (x < 0.0)
    x+= n;
  while (x >= n)
    x-= n;

 
  if (n%2)
  {
    fprintf(stderr, "Error, odd numbers of bins not implemented in sinc_interp!!\n"); // possible but uglier
    exit(1);
  }

  int i0 = (int)floor(x+0.5);
  double delta = x - (double)i0;
  int i;

#if 1
  if (fabs(delta) < 1.0e-4)
  {

    // The equations below explode when x is nearly integer.
    // In that case, several large terms cancel and one ends up with,
    // for the peak contribution:
    double result =  (i0%2 ? -1.0: 1.0) * -f[i0] * (M_PI*M_PI/3.0 * (1.0 + 2.0/(n*n))
      + (M_PI*M_PI*(1.0+2.0*M_PI*M_PI/(3.0*n*n))) * delta * delta);
    // for the remaining terms, the following is a good appoximation:
    for (i=0; i < n; i++)
    {
      if (i!=i0)
      {
	double csc = 1.0/sin(M_PI*(x-i)/n);
	result +=  (i%2 ? -1.0: 1.0) * -f[i]*M_PI*M_PI 
	  * 2.0 * csc*csc / (1.0*n*n);
     }
    } 
    result *= (i0%2 ? -1.0: 1.0);
    return result;
//     fprintf(stderr, "Result 1 = %lg\n", result);
    //       exit(1);
  } 
#endif

  // Second Derivative of sinc_interp(), obtained using Mathematica
  double result = 0.0;
  double sinpix = sin(M_PI*(x - 2.0*floor(0.5*x)));
  double cospix = cos(M_PI*(x - 2.0*floor(0.5*x)));
  for (i=0; i < n; i++)
  {
    double csc = 1.0/sin(M_PI*(x-i)/n);
    double cscsq = csc*csc;
    double cot = 1.0/tan(M_PI*(x-i)/n);
    
    result += (i%2 ? -1.0: 1.0) * -f[i]*M_PI*M_PI * 
      (2.0*n*cospix*cscsq+cot*(1.0*n*n-2.0*cscsq)*sinpix)/(1.0*n*n*n);

//   if (fabs(delta) < 1.0e-6 && i0==41)
//     printf("%d %lg APPROX2\n", i, (i%2 ? -1.0: 1.0) * -M_PI*M_PI * 
// 	   (2.0*n*cospix*cscsq+cot*(1.0*n*n-2.0*cscsq)*sinpix)/(1.0*n*n*n));
  }
//   if (fabs(delta) < 1.0e-6 && i0==40)
//   {
//     fprintf(stderr, "Result 2 = %lg\n", result);
//     double h = 0.05;
//     fprintf(stderr, "Numerical: %lg\n",
// 	    (sinc_interp(f, x-h, n) 
// 	     - 2.0*sinc_interp(f, x, n)
// 	     +sinc_interp(f, x+h, n)) / (h*h));
//     exit(1); 

//  }
  return result ;
}



#define SHFT(a,b,c,d) (a)=(b);(b)=(c);(c)=(d);
#define SIGN(a,b) ((b) >= 0.0 ? fabs(a) : -fabs(a))

static void
find_peak(float *f, unsigned n, 
	  double *xmax, float *ymax)
{
  // find peak bin
  unsigned imax = std::max_element(f, f+n)-f;

  // Rest of code is hacked version of Numerical Recipes' "brent".
  double ax = imax-1.0;
  double bx = imax;
  double cx = imax+1.0; 
  double tol = 1.0e-8; // >> sqrt of double precision


  int iter;
  double a,b,d=0,etemp,p,q,r,tol1,tol2,u,v,w,x,xm;
  float fu,fv,fw,fx;
  double e=0.0;
  const int ITMAX = 100;
  const double CGOLD = 0.3819660;
  const double ZEPS = 1.0e-10;

  a=(ax < cx ? ax : cx);
  b=(ax > cx ? ax : cx);
  x=w=v=bx;
  fw=fv=fx=-sinc_interp(f, x, n);
  for (iter=1;iter<=ITMAX;iter++) {
    xm=0.5*(a+b);
    tol2=2.0*(tol1=tol*fabs(x)+ZEPS);
    if (fabs(x-xm) <= (tol2-0.5*(b-a))) 
    {
      *xmax = x;
      *ymax = -fx;
      return ;
    }
    if (fabs(e) > tol1) {
      r=(x-w)*(fx-fv);
      q=(x-v)*(fx-fw);
      p=(x-v)*q-(x-w)*r;
      q=2.0*(q-r);
      if (q > 0.0) p = -p;
      q=fabs(q);
      etemp=e;
      e=d;
      if (fabs(p) >= fabs(0.5*q*etemp) || p <= q*(a-x) || p >= q*(b-x))
	d=CGOLD*(e=(x >= xm ? a-x : b-x));
      else {
	d=p/q;
	u=x+d;
	if (u-a < tol2 || b-u < tol2)
	  d=SIGN(tol1,xm-x);
      }
    } else {
      d=CGOLD*(e=(x >= xm ? a-x : b-x));
    }
    u=(fabs(d) >= tol1 ? x+d : x+SIGN(tol1,d));
    fu=-sinc_interp(f, u, n);
    if (fu <= fx) {
      if (u >= x) a=x; else b=x;
      SHFT(v,w,x,u)
	SHFT(fv,fw,fx,fu)
	} else {
	  if (u < x) a=u; else b=u;
	  if (fu <= fw || w == x) {
	    v=w;
	    w=u;
	    fv=fw;
	    fw=fu;
	  } else if (fu <= fv || v == x || v == w) {
	    v=u;
	    fv=fu;
	  }
	}
  }
  fprintf(stderr, "Internal error!!\n");
  exit(1);
}

 
  

Estimate<double> Pulsar::SincInterpShift::get_shift () const
{
  unsigned nbin_std = standard->get_nbin();
  unsigned nbin_obs = observation->get_nbin();
  unsigned i, nbin = std::min(nbin_std, nbin_obs), nby2 = nbin/2, ncoeff=nby2+2;
  double mismatch_shift=0.0;

  if (nbin_std!=nbin_obs)
  {
    // if different numbers of bins, there is an extra offset equal to
    // the offset between the centres of bin 0 of each profile, i.e.
    mismatch_shift = 0.5/nbin_std - 0.5/nbin_obs;
  }

  // compute the cross-correlation
  // Note, in case of number of bins mismatch, we compute the full FFT of
  // each and only use those coefficients they have in common
  std::complex<float> *obs_spec = new std::complex<float> [nbin_obs/2+2];
  std::complex<float> *std_spec = new std::complex<float> [nbin_std/2+2];
  std::complex<float> *ccf_spec = new std::complex<float> [ncoeff];
  const std::complex<float> zero(0.0, 0.0); 
  float *ccf = new float [nbin];

  FTransform::frc1d (nbin_obs, (float*)obs_spec, observation->get_amps());
  FTransform::frc1d (nbin_std, (float*)std_spec, standard->get_amps());

  // Zap harmonics of periodic spikes if necessary
  int nadd = nby2-1; //keep track of how many coefficients are used
  if (zap_period > 1)
  {
    int freq = nbin / zap_period;
    for (i=freq; i < nby2; i+=freq)
    {
      obs_spec[i] = zero;
      nadd--;
    }
  }

  for (i=1; i < nby2; i++)
    ccf_spec[i] = obs_spec[i]*conj(std_spec[i]);
  
  //fprintf(stderr, "DC=%f \n", real(obs_spec[0]));
  ccf_spec[0] = zero; // ignore DC components
  ccf_spec[nby2] = zero; // Ignore Nyquist, it has no phase information
  
  FTransform::bcr1d (nbin, ccf, (float*)ccf_spec);

//   fprintf(stderr, "ccf[0] = %g\n", ccf[0]);

  double maxbin;
  float peakval;
  find_peak(ccf, nbin, &maxbin, &peakval);

  peakval /= nbin*nbin; // correct for scaling in FFTs
//   fprintf(stderr, "peakval = %g\n", peakval);

  // Get the RMS by a means analogous to Taylor's frequency domain method.
  // We have obs = b.std + noise1, so the CCF peak, i.e. the covariance,
  //    covar = b var_std + noise2
  // and the noise variance can be got at via
  //    var_obs = b^2 var_std + var_noise1 .
  // This introduces noise in the covariance (via noise2), which will have
  // a variance of 
  //    var_noise2 = var_noise1 . var_std / N
  // Recall that 
  // so the variance of the ccf peak is
  //  (var_obs - b^2 var_std) . var std /N ... substitute b = covar / var_std
  //  = (var_obs.var_std - covar^2 ) / N

  //  variance_noise = ACF_obs(0) + b^2ACF_std(0) - 2 b CCF(tau)
  //  where b = CCF(tau) / ACF_std(0)
  double variance_obs=0.0, variance_std=0.0;
 
  for (i=1; i < nbin/2; i++)
  {
    variance_obs += norm(obs_spec[i]);
    variance_std += norm(std_spec[i]);
  }

  //   // correct for FFT scaling, plus use of only +ve frequencies
  variance_obs *= 2.0/(nbin*nbin);
  variance_std *= 2.0/(nbin*nbin);

  double rms = (variance_obs*variance_std-peakval*peakval)/(nadd*2);

  // Arrgh for some reason this just doesn't work!! Instead copy
  // the result from chi squared minimisation, which is related to our
  // peakval by constant - 2b/var_noise1 * peakval. The delta-Chi-sq=1
  // points therefore correspond to 
  //      delta_peakval = var_noise/(2b)
  // From var_noise = var_obs - b^2 var_std, we get
  //   delta_peakval = 1/2 (var_obs/b - b var_std)
  // Substitute b = peakval / var_std:
  //  rms =-0.5*(variance_obs*variance_std/peakval - peakval);
  //  which is related to my calculation above by
  rms *= 0.5/peakval; // root 2 is fudge factor!! figure it out!!

  if (rms < 0.0)
  {
    fprintf(stderr, "Ooopsy\n");
  }

//   double scale = 0.5*peakval / variance_std;
//   double rms = variance_obs + scale*scale*variance_std - scale * peakval;
//   // above = var_obs + 1/4 covar^2/var_std - 1/2 covar^2/var_std
//   //       = var_obs - 1/2 covar^2/var_std


//   rms *= 1.0/nbin;

//      printf("%lg %lg %g %lg VAR\n", variance_obs, variance_std, peakval, rms);
//   printf("%lg %lg  CMP\n", 
// 	 (variance_obs*variance_std-peakval*peakval)/nadd,
// 	 rms);

 
  // now we need the second derivative of the ccf to get an idea of the 
  // error in its peak .. 
  // (note correct again for scaling in ccf)
  double second_deriv 
    = sinc_interp_second_derivative(ccf, maxbin, nbin) / (nbin*nbin);

#if 0
  if (second_deriv >= 0.0)
  {
    fprintf(stderr, "Poop %lf %lg\n", second_deriv, maxbin-floor(maxbin+0.5));
    double x;
    printf("\n");
    for (x=maxbin-1.0; x <= maxbin+1.0; x+=0.01)
      printf("%lf %f POOP\n", x, sinc_interp(ccf, x, nbin));
  }
#endif
  // the ccf in the vicinity of the peak is 
  //    ccf = ccf(maxbin) + 1/2 second_deriv (m-maxbin)^2
  // then, the change in maxbin needed to change the ccf by 1 sigma is
  //  (m-maxbin)^2 = 2. rms / second_deriv
  double sigma_maxbin =  sqrt(2.0 * rms / -second_deriv);

 //    printf("%lg %lg %lg SIGMASIS\n", rms, -second_deriv, scale);

  
     
  double shift = maxbin / nbin - mismatch_shift;
  double sigma_shift = sigma_maxbin / nbin;
  if (shift >=0.5)
    shift -= 1.0; 

 
  delete [] ccf;
  delete [] ccf_spec;
  delete [] std_spec;
  delete [] obs_spec;

  return Estimate<double>(shift, sigma_shift*sigma_shift);
}

