/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/ConvertIsolated.h"
#include "Pulsar/PhaseWeight.h"

using namespace std;

// #define _DEBUG 1

//! Default constructor
Pulsar::ConvertIsolated::ConvertIsolated ()
{
  neighbourhood = 0.01;
  like_fraction = 1.0;
  test_value = 0.0;
  convert_value = 1.0;
}

Pulsar::ConvertIsolated* Pulsar::ConvertIsolated::clone () const
{
  return new ConvertIsolated (*this);
}

//! Set the smoothing neighbourhood fraction
void Pulsar::ConvertIsolated::set_neighbourhood (float fraction)
{
  if (fraction <= 0.0 || fraction >= 1.0)
    throw Error (InvalidParam, "Pulsar::ConvertIsolated::set_neighbourhood",
		 "invalid neighbourhood fraction = %lf", fraction);
  neighbourhood = fraction;
}

//! Get the smoothing neighbourhood fraction
float Pulsar::ConvertIsolated::get_neighbourhood () const
{
  return neighbourhood;
}

void Pulsar::ConvertIsolated::set_like_fraction (float fraction)
{
  if (fraction <= 0.0 || fraction > 1.0)
    throw Error (InvalidParam, "Pulsar::ConvertIsolated::set_like_fraction",
		 "invalid like_fraction fraction = %lf", fraction);
  like_fraction = fraction;
}

float Pulsar::ConvertIsolated::get_like_fraction () const
{
  return like_fraction;
}

// #define _DEBUG 1

void Pulsar::ConvertIsolated::calculate (PhaseWeight* weight)
{
  unsigned nbin = input_weight->get_nbin();
  unsigned ncheck = unsigned( nbin * neighbourhood );
  unsigned nlike = unsigned( nbin * neighbourhood * like_fraction );

  if (ncheck < 2)
    ncheck = 2;

  if (nlike < 1)
    nlike = 1;

#ifdef _DEBUG
  cerr << "Pulsar::ConvertIsolated::calculate ncheck=" << ncheck << endl;
#endif

  for (unsigned ibin=0; ibin < nbin; ibin++)
    (*weight)[ibin] = (*input_weight)[ibin];

  for (unsigned ibin=0; ibin < nbin; ibin++) {

#ifdef _DEBUG
    cerr << ibin << " " << (*input_weight)[ibin] << ": ";
#endif

    if ( (*weight)[ibin] != test_value ) {
#ifdef _DEBUG
      cerr << endl;
#endif
      continue;
    }

    // count the equal elements following this bin
    unsigned nright = 0;
    for (unsigned jbin=0; jbin < ncheck; jbin++) {
      unsigned itest = (ibin+jbin+nbin+1) % nbin;
#ifdef _DEBUG
      cerr << " " << itest << "=" << (*input_weight)[ itest ];
#endif
      if ((*input_weight)[ itest ] == test_value)
	nright ++;
    }
#ifdef _DEBUG
    cerr << " : " << nright << " ";
#endif

    if (nright >= nlike) {
#ifdef _DEBUG
      cerr << "r" << endl;
#endif
      continue;
    }

    // count the equal elements preceding this bin
    unsigned nleft = 0;
    for (unsigned jbin=0; jbin < ncheck; jbin++) {
      unsigned itest = ((ibin+jbin+nbin) - ncheck) % nbin;
#ifdef _DEBUG
      cerr << " " << itest << "=" << (*input_weight)[ itest ];
#endif
      if ( (*input_weight)[ itest ] == test_value)
	nleft ++;
    }

#ifdef _DEBUG
    cerr << " : " << nleft << " ";
#endif

    if (nleft >= nlike) {
#ifdef _DEBUG
      cerr << "l" << endl;
#endif
      continue;
    }

#ifdef _DEBUG
    cerr << "Pulsar::ConvertIsolated::calculate converting ibin=" 
	 << ibin << "/" << nbin << endl;
#endif

    (*weight)[ibin] = convert_value;

  }
}
