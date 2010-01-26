/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/MaskSmooth.h"
#include "Pulsar/PhaseWeight.h"

using namespace std;

//! Default constructor
Pulsar::MaskSmooth::MaskSmooth ()
{
  window = 0.01;
  masked = 0.50;
}

//! Set the smoothing window fraction
void Pulsar::MaskSmooth::set_window (double fraction)
{
  if (fraction <= 0.0 || fraction >= 1.0)
    throw Error (InvalidParam, "Pulsar::MaskSmooth::set_window",
		 "invalid window fraction = %lf", fraction);
  window = fraction;
}

//! Get the smoothing window fraction
double Pulsar::MaskSmooth::get_window () const
{
  return window;
}

//! Set the fraction of masked neighbours required
void Pulsar::MaskSmooth::set_masked (double fraction)
{
  if (fraction <= 0.0 || fraction >= 1.0)
    throw Error (InvalidParam, "Pulsar::MaskSmooth::set_masked",
		 "invalid masked fraction = %lf", fraction);
  masked = fraction;
}

//! Get the fraction of masked neighbours required
double Pulsar::MaskSmooth::get_masked () const
{
  return masked;
}

// #define _DEBUG 1

void Pulsar::MaskSmooth::calculate (PhaseWeight* weight)
{
  unsigned nbin = input_weight->get_nbin();
  unsigned iwindow = nbin * window * 0.5;
  unsigned imasked = iwindow * masked;

  if (imasked == 0) {
    imasked = 1;
    iwindow = unsigned (float(imasked)/masked);
  }

#ifdef _DEBUG
  cerr << "Pulsar::MaskSmooth::calculate iwindow=" << iwindow 
       << " imasked=" << imasked << endl;
#endif

  for (unsigned ibin=0; ibin < nbin; ibin++) {

#ifdef _DEBUG
    cerr << ibin << " " << (*input_weight)[ibin] << ": ";
#endif

    if ((*input_weight)[ibin]) {
      (*weight)[ibin] = true;
#ifdef _DEBUG
      cerr << "t" << endl;
#endif
      continue;
    }

    // count the masked bins before this bin
    unsigned nmasked = 0;
    for (unsigned jbin=0; jbin < iwindow; jbin++)
      if ((*input_weight)[ (ibin + jbin - iwindow + nbin ) % nbin ])
	nmasked ++;

#ifdef _DEBUG
    cerr << nmasked << " ";
#endif

    if (nmasked < imasked) {
#ifdef _DEBUG
      cerr << endl;
#endif
      continue;
    }

    // count the masked bins after this bin
    nmasked = 0;
    for (unsigned jbin=0; jbin < iwindow; jbin++)
      if ((*input_weight)[ (ibin + jbin + 1 + nbin ) % nbin ])
	nmasked ++;

#ifdef _DEBUG
    cerr << nmasked << " ";
#endif

    if (nmasked < imasked) {
#ifdef _DEBUG
      cerr << endl;
#endif
      continue;
    }

#ifdef _DEBUG
    cerr << "Pulsar::MaskSmooth::calculate filling ibin=" << ibin << endl;
#endif
    (*weight)[ibin] = true;

  }
}
