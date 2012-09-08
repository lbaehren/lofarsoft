/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/LastSignificant.h"
#include "Pulsar/Profile.h"

using namespace std;

// #define _DEBUG 1

//! Default constructor
Pulsar::LastSignificant::LastSignificant ()
{
  cutoff_sigma = 3.0;
  consecutive = 3;
  last_significant = 0;
}

//! Set the cut-off threshold as a multiple of the rms
void Pulsar::LastSignificant::set_threshold (float sigma)
{
  cutoff_sigma = sigma;
}

float Pulsar::LastSignificant::get_threshold () const
{
  return cutoff_sigma;
}

void Pulsar::LastSignificant::set_consecutive (unsigned bins)
{
  consecutive = bins;
}

//! Get the number of consecutive bins
unsigned Pulsar::LastSignificant::get_consecutive () const
{
  return consecutive;
}

//! Get the last signficant bin
unsigned Pulsar::LastSignificant::get () const
{
  return last_significant;
}

//! Reset the last significant phase bin
void Pulsar::LastSignificant::reset ()
{
  last_significant = 0;
}

//! Find the last signficant bin
void Pulsar::LastSignificant::find (const Profile* psd, double rms)
{
  find( psd->get_nbin(), psd->get_amps(), rms );
}

//! Find the last significant bin
void Pulsar::LastSignificant::find (const std::vector<float>& psd, double rms)
{
  find( psd.size(), &(psd[0]), rms);
}

//! Find the last significant bin
void
Pulsar::LastSignificant::find (unsigned nbin, const float* psd, double rms)
{
  double threshold = rms * cutoff_sigma;
  unsigned count = 0;

#ifdef _DEBUG
  cerr << "RMS=" << rms << " THRESHOLD=" << threshold  << endl;
#endif

  for (unsigned ibin=1; ibin<nbin; ibin++)
  {

    if (psd[ibin] > threshold)
      count ++;
    else
      count = 0;

#ifdef _DEBUG
    cerr << count << " " << ibin << " " << psd[ibin]  <<endl;
#endif

    if (count >= consecutive && ibin > last_significant)
      last_significant = ibin;

  }
  
#ifdef _DEBUG
  cerr << "last_significant = " << last_significant << endl;
#endif
  
}

