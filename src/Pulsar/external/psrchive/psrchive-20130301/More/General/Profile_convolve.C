/***************************************************************************
 *
 *   Copyright (C) 2002 by Aidan Hotan
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
using namespace std;
#include "Pulsar/Profile.h"
#include "Error.h"
#include "FTransform.h"

#include <memory>

void Pulsar::Profile::convolve (const Profile* profile)
{
  convolve (profile, -1);
}

void Pulsar::Profile::correlate (const Profile* profile)
{
  convolve (profile, 1);
}

void Pulsar::Profile::correlate_normalized (const Profile* profile)
{
  /*
    Note that the mean and variance are computed over all phase bins,
    not only the off-pulse region.
  */
  double mean = -1.0, var1=-1.0, var2 =-1.0, varmean = -1.0;
  this->stats (&mean, &var1, &varmean, 0, this->get_nbin());
  this->offset (-mean);

  Reference::To<Pulsar::Profile> temp = profile->clone();

  temp->stats (&mean, &var2, &varmean, 0, temp->get_nbin());
  temp->offset (-mean);

  this->correlate (temp);
  this->scale (1.0 / sqrt(var1) / sqrt(var2) / this->get_nbin());
}

void Pulsar::Profile::convolve (const Profile* profile, int dir)
{
  unsigned nbin = get_nbin();
  unsigned mbin = profile->get_nbin();

  vector<double> temp (nbin, 0.0);

  const float* nptr = get_amps();
  const float* mptr = profile->get_amps();

  for (unsigned ibin=0; ibin < nbin; ibin++) {
    for (unsigned jbin=0; jbin < mbin; jbin++) {
      unsigned n_bin = (ibin+jbin)%nbin;
      unsigned m_bin = (mbin+dir*jbin)%mbin;
      temp[ibin] += nptr[n_bin] * mptr[m_bin];
    }
  }

  set_amps (temp);
}


void Pulsar::Profile::fft_convolve (const Profile* p1)
{
  unsigned bins = get_nbin();

  if (bins != p1->get_nbin())
    throw Error (InvalidParam, "Profile::fft_convolve", 
		 "profile nbin values not equal");

  vector<float> temp1( bins+2 );
  vector<float> temp2( bins+2 );
  vector<float> resultant( bins+2 );

  FTransform::frc1d (bins, &temp1[0], get_amps());
  FTransform::frc1d (bins, &temp2[0], p1->get_amps());

  // cast the float* arrays to complex<float>*
  complex<float>* c1 = (complex<float>*) &temp1[0];
  complex<float>* c2 = (complex<float>*) &temp2[0];
  complex<float>* r = (complex<float>*) &resultant[0];

  unsigned ncomplex = bins/2+1;

  // Complex multiplication of the elements
  for (unsigned i = 0; i < ncomplex; i++)
    r[i] = c1[i]*c2[i];

  // Transform back to the time domain to get the convolved solution

  vector<float> solution (bins);

  FTransform::bcr1d (bins, &(solution[0]), &resultant[0]);

  set_amps(solution);
}

