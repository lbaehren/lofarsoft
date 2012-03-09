/***************************************************************************
 *
 *   Copyright (C) 2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/StokesCovariance.h"
#include "Pulsar/FourthMoments.h"

#include "Pulsar/Profile.h"
#include "Pulsar/PhaseWeight.h"

#include "Pauli.h"
#include "Error.h"

#include <assert.h>

using namespace std;

/*! 4 Stokes parameters -> (4*(4+1))/2 = 10 covariances */
const unsigned Pulsar::StokesCovariance::nmoment = 10;

//
//
//
Pulsar::StokesCovariance::StokesCovariance (FourthMoments* fourth)
{
  if (!fourth)
    return;

  if (fourth->get_size() != nmoment)
    throw Error (InvalidParam, "Pulsar::StokesCovariance ctor",
                 "FourthMoments size=%u != expected=%u",
                 fourth->get_size(), nmoment);

  covariance = fourth;
}

//
//
//
Pulsar::StokesCovariance* Pulsar::StokesCovariance::clone () const
{
  return new StokesCovariance (covariance->clone());
}

//
//
//
Pulsar::StokesCovariance::~StokesCovariance ()
{
  if (Profile::verbose)
    cerr << "Pulsar::StokesCovariance destructor" << endl;
}

//
//
//
void Pulsar::StokesCovariance::resize (unsigned nbin)
{
  covariance->resize (nbin);
}

//
//
//
unsigned Pulsar::StokesCovariance::get_nbin () const
{
  return covariance->get_nbin();
}

//
//
//
const Pulsar::Profile* 
Pulsar::StokesCovariance::get_Profile (unsigned i) const
{
  return covariance->get_Profile(i);
}

//
//
//
Pulsar::Profile* 
Pulsar::StokesCovariance::get_Profile (unsigned i)
{
  return covariance->get_Profile(i);
}

//
//
//
Matrix<4,4,double> 
Pulsar::StokesCovariance::get_covariance (unsigned ibin) const
{
  if (ibin >= get_nbin())
    throw Error (InvalidRange, "StokesCovariance::get_covariance",
		 "ibin=%d >= nbin=%d", ibin, get_nbin());

  Matrix<4,4,double> result;
  unsigned index=0;

  for (unsigned i=0; i<4; i++)
    for (unsigned j=i; j<4; j++)
    {
      result[i][j] = result[j][i] = get_Profile(index)->get_amps()[ibin];
      index ++;
    }

  assert (index == nmoment);
  return result;
}

//
//
//
void Pulsar::StokesCovariance::set_covariance (unsigned ibin, 
					       const Matrix<4,4,double>& C)
{
  if (ibin >= get_nbin())
    throw Error (InvalidRange, "StokesCovariance::set_covariance",
		 "ibin=%d >= nbin=%d", ibin, get_nbin());

  unsigned index=0;

  for (unsigned i=0; i<4; i++)
    for (unsigned j=i; j<4; j++)
    {
      get_Profile(index)->get_amps()[ibin] = C[i][j];
      index ++;
    }

  assert (index == nmoment);
}

void Pulsar::StokesCovariance::scale (double scale)
{
  covariance->scale (scale);
}

void Pulsar::StokesCovariance::rotate_phase (double phase)
{
  covariance->rotate_phase (phase);
}

//
//
//
void Pulsar::StokesCovariance::transform (const Jones<double>& response)
{
  if (Profile::verbose)
    cerr << "Pulsar::StokesCovariance::transform response="
	 << response << endl;
  
  transform (Mueller(response));
}

//
//
//
void Pulsar::StokesCovariance::transform (const Matrix<4,4,double>& response)
{
  if (Profile::verbose)
    cerr << "Pulsar::StokesCovariance::transform response=\n" 
	 << response << endl;

  const unsigned nbin = get_nbin();

  for (unsigned ibin = 0; ibin < nbin; ibin++)
    set_covariance (ibin, response* get_covariance(ibin) *transpose(response));
}


