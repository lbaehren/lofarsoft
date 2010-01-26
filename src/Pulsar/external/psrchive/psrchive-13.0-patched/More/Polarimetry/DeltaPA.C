/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/DeltaPA.h"
#include "Pulsar/PolnProfile.h"
#include "templates.h"

#include "stdio.h"
#include <iostream>
using namespace std;

// #define _DEBUG 1

template <typename T, typename U = T>
class MeanArc
{

public:

  //! Default constructor
  MeanArc () { covar = 0; }

  //! Copy constructor
  MeanArc (const MeanArc& mean)
  { operator = (mean); }

  //! Assignment operator
  const MeanArc& operator= (const MeanArc& mean)
  { cosine = mean.cosine; sine = mean.sine; covar = mean.covar; return *this; }

  //! Addition operator
  const MeanArc& operator+= (const MeanArc& d)
  { cosine += d.cosine; sine += d.sine; covar += d.covar; return *this; }

  //! Add data
  void add (const Estimate<T,U>& x0, const Estimate<T,U>& y0, 
            const Estimate<T,U>& x1, const Estimate<T,U>& y1)
  {
    Estimate<T,U> cosarc = x0*x1 + y0*y1; cosine += cosarc;
    Estimate<T,U> sinarc = x0*y1 - y0*x1; sine += sinarc;

    T norm = 1.0 / (cosarc.get_variance() * sinarc.get_variance());

    // del cosarc del x0 * del sinarc del x0 * var(x0)
    covar += x1.get_value() * y1.get_value() * x0.get_variance() * norm;
 
    // del cosarc del y0 * del sinarc del y0 * var(y0)
    covar -= y1.get_value() * x1.get_value() * y0.get_variance() * norm;
 
    // del cosarc del x1 * del sinarc del x1 * var(x1)
    covar -= x0.get_value() * y0.get_value() * x1.get_variance() * norm;
 
    // del cosarc del y1 * del sinarc del y1 * var(y1)
    covar += y0.get_value() * x0.get_value() * y1.get_variance() * norm;

#ifdef _DEBUG
    cerr << "CC=" << 1.0/cosine.inv_var << " SS=" << 1.0/sine.inv_var
	 << " CS=" << covar << " norm=" << norm << endl;
#endif

  }

  //!
  Estimate<T,U> get_Estimate () const
  {
    if (sine.norm_val==0 && cosine.norm_val==0) return Estimate<T,U>(0,0);

    Estimate<T,U> sbar = sine.get_Estimate();
    Estimate<T,U> cbar = cosine.get_Estimate();

    Estimate<T,U> arc = atan2 (sbar, cbar);

    T covar_sbarcbar = covar * sbar.get_variance() * cbar.get_variance();

    T one = (sbar.val*sbar.val + cbar.val*cbar.val);

    T delarc_delsbar = cbar.get_value() / one;
    T delarc_delcbar = -sbar.get_value() / one; 

    arc.var += 2.0 * covar_sbarcbar * delarc_delsbar * delarc_delcbar;

    return arc;
  }

  Estimate<T,U> get_sin () const
  { return sine.get_Estimate(); }

  Estimate<T,U> get_cos () const
  { return cosine.get_Estimate(); }

protected:

  //! The average cosine
  MeanEstimate<T,U> cosine;

  //! The average sine
  MeanEstimate<T,U> sine;

  //! The covariance
  T covar;

};

//! Set the phase bins to be included in the mean
void Pulsar::DeltaPA::set_include (const std::vector<unsigned>& bins)
{
  include_bins = bins;
}

//! Set the phase bins to be excluded exclude the mean
void Pulsar::DeltaPA::set_exclude (const std::vector<unsigned>& bins)
{
  exclude_bins = bins;
}

#include "Pulsar/GaussianBaseline.h"
#include "Pulsar/PhaseWeight.h"

Estimate<double> 
Pulsar::DeltaPA::get (const PolnProfile* p0, const PolnProfile* p1) const
{
  if (!p0 || !p1)
    throw Error (InvalidState, "Pulsar::DeltaPA::get", "no data");

  Profile linear0;
  p0->get_linear (&linear0);

  Profile linear1;
  p1->get_linear (&linear1);

  Pulsar::GaussianBaseline mask;
  Pulsar::PhaseWeight weight;
    
  mask.set_Profile (p0->get_Profile(0));
  mask.get_weight (&weight);

  double mu_q0 = 0.0, var_q0 = 0.0;
  weight.stats (p0->get_Profile(1), &mu_q0, &var_q0);
  double mu_u0 = 0.0, var_u0 = 0.0;
  weight.stats (p0->get_Profile(2), &mu_u0, &var_u0);

  double mu_q1 = 0.0, var_q1 = 0.0;
  weight.stats (p1->get_Profile(1), &mu_q1, &var_q1);
  double mu_u1 = 0.0, var_u1 = 0.0;
  weight.stats (p1->get_Profile(2), &mu_u1, &var_u1);

  float cutoff0 = threshold * sqrt (0.5*(var_q0 + var_u0));
  float cutoff1 = threshold * sqrt (0.5*(var_q1 + var_u1));

#ifdef _DEBUG
  cerr << "1: var q=" << var_q1 << " u=" << var_u1 << " cut=" 
       << cutoff1 << endl;
#endif

  const float *q0 = p0->get_Profile(1)->get_amps(); 
  const float *u0 = p0->get_Profile(2)->get_amps(); 

  const float *q1 = p1->get_Profile(1)->get_amps(); 
  const float *u1 = p1->get_Profile(2)->get_amps(); 

  unsigned nbin = p0->get_nbin();
  used_bins = 0;

  double cos_delta_PA = 0.0;
  double sin_delta_PA = 0.0;

  MeanArc<double> arc;

  FILE* fptr = fopen ("delta_pa.txt", "w");
  
  for (unsigned ibin=0; ibin<nbin; ibin++)
  {
    if (include_bins.size() && !found (ibin, include_bins))
      continue;

    if (exclude_bins.size() && found (ibin, exclude_bins))
      continue;

    if (linear0.get_amps()[ibin] < cutoff0 ||
	linear1.get_amps()[ibin] < cutoff1)
      continue;

    cos_delta_PA += q0[ibin]*q1[ibin] + u0[ibin]*u1[ibin];
    sin_delta_PA += q0[ibin]*u1[ibin] - q1[ibin]*u0[ibin];

    MeanArc<double> arc1;
    arc1.add( Estimate<double> (q0[ibin], var_q0),
	      Estimate<double> (u0[ibin], var_u0),
	      Estimate<double> (q1[ibin], var_q1),
	      Estimate<double> (u1[ibin], var_u1) );

    Estimate<double> delta_pa = arc1.get_Estimate();

    if (fptr)
      fprintf (fptr, "%u  %lf  %lf\n", ibin,
	       delta_pa.get_value(), delta_pa.get_error());

#ifdef _DEBUG
    cerr << "ibin=" << ibin << " dPA=" << delta_pa << endl;
#endif

    arc += arc1; 
    used_bins ++;
  }

  if (fptr)
    fclose (fptr);

  if (used_bins == 0)
    throw Error( InvalidParam, "Pulsar::DeltaPA::get",
		 "linear polarization did not exceed thresholds\n\t"
		 "threshold=%f  cutoff0=%f  cutoff1=%f",
		 threshold, cutoff0, cutoff1);

  double one = cos_delta_PA*cos_delta_PA + sin_delta_PA*sin_delta_PA;
  cos_delta_PA /= one;
  sin_delta_PA /= one;

  double var_delta_PA = 0.0;

  for (unsigned ibin=0; ibin<nbin; ibin++)
  {
    if (include_bins.size() && !found (ibin, include_bins))
      continue;

    if (exclude_bins.size() && found (ibin, exclude_bins))
      continue;

    if (linear0.get_amps()[ibin] < cutoff0 ||
	linear1.get_amps()[ibin] < cutoff1)
      continue;

    double del = 0;

    // del delta_PA del q0
    del = cos_delta_PA * u1[ibin] - sin_delta_PA * q1[ibin];
    var_delta_PA += del * del * var_q0;

    // del delta_PA del u0
    del = -cos_delta_PA * q1[ibin] - sin_delta_PA * u1[ibin];
    var_delta_PA += del * del * var_u0;
    
    // del delta_PA del q1
    del = -cos_delta_PA * u0[ibin] - sin_delta_PA * q0[ibin];
    var_delta_PA += del * del * var_q1;
    
    // del delta_PA del u1
    del = cos_delta_PA * q0[ibin] - sin_delta_PA * u0[ibin];
    var_delta_PA += del * del * var_u1;
    
  }

  Estimate<double> radians( atan2(sin_delta_PA,cos_delta_PA), var_delta_PA );

  cerr << "radians old=" << radians << " new=" << arc.get_Estimate() << endl;

  // return the answer in P.A. = 1/2 radians
  return 0.5 * arc.get_Estimate();

}
