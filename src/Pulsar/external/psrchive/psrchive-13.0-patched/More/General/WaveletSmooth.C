/***************************************************************************
 *
 *   Copyright (C) 2008 by Paul Demorest
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/WaveletSmooth.h"
#include "Pulsar/WaveletTransform.h"
#include "Pulsar/Profile.h"

#include <algorithm>

using namespace std;

Pulsar::WaveletSmooth::WaveletSmooth ()
{
  // These seem like good defaults
  thresh = Hard;
  set_wavelet(gsl_wavelet_daubechies, 8);
  cutoff_factor = 1.3;
}

Pulsar::WaveletSmooth::~WaveletSmooth ()
{
}

void Pulsar::WaveletSmooth::set_wavelet(const string wstring) {
  wt.set_wavelet(wstring);
}

void Pulsar::WaveletSmooth::set_wavelet(const gsl_wavelet_type *t, int order,
    bool decimate) {
  wt.set_type(t);
  wt.set_order(order);
  wt.set_decimate(decimate);
}

void Pulsar::WaveletSmooth::transform(Pulsar::Profile *prof) {

  // Do wavelet transform
  wt.transform(prof);

  // Calculate noise level from median of highest level
  // wavelet coeffs
  std::vector<double> wc;
  wc.resize(wt.get_ncoeff(wt.get_log2_npts()-1));
  for (unsigned i=0; i<wc.size(); i++) 
    wc[i] = fabs(wt.get_data(wt.get_log2_npts()-1,i));
  std::nth_element(wc.begin(), wc.begin() + wc.size()/2, wc.end());
  sigma = (1.0/0.6745)*wc[wc.size()/2];

  // Threshold wavelet coeffs
  ncoeff = wt.get_ncoeff();
  cutoff = cutoff_factor*sqrt(2.0 * log((double)prof->get_nbin()));
  unsigned idx0 = wt.get_decimate() ? 1 : 0;
  for (unsigned i=idx0; i<wt.get_ncoeff(); i++) {
    if (thresh==Hard) wt.get_data()[i] = thresh_hard(wt.get_data(i));
    else if (thresh==Soft) wt.get_data()[i] = thresh_soft(wt.get_data(i));
    else 
      throw Error (InvalidState, "Pulsar::WaveletSmooth::transform",
          "Unrecognized threshold setting");
    if (wt.get_data(i)==0.0) ncoeff--;
  }

  // Inverse transform, replace input data
  wt.invert();
  for (unsigned i=0; i<prof->get_nbin(); i++) 
    prof->get_amps()[i] = wt.get_data(i);

}

double Pulsar::WaveletSmooth::thresh_hard(double in) {
  if (fabs(in) > cutoff*sigma) return in;
  else return 0.0;
}

double Pulsar::WaveletSmooth::thresh_soft(double in) {
  if (fabs(in) > cutoff*sigma) {
    if (in>0.0) return in - cutoff*sigma;
    else return in + cutoff*sigma;
  } else 
    return 0.0;
}

