/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/MaskSmooth.h"
#include "Pulsar/PhaseWeight.h"

//#define _DEBUG 1

using namespace std;

//! Default constructor
Pulsar::MaskSmooth::MaskSmooth ()
{
  turns = 0.01;
  masked_fraction = 0.50;

  bins = 0;
  masked_bins = 0;

  mask_value = 0.0;
}

Pulsar::MaskSmooth* Pulsar::MaskSmooth::clone () const
{
  return new MaskSmooth (*this);
}

void Pulsar::MaskSmooth::set_turns (double fraction)
{
  if (fraction <= 0.0 || fraction >= 1.0)
    throw Error (InvalidParam, "Pulsar::MaskSmooth::set_turns",
		 "invalid window fraction = %lf", fraction);
  turns = fraction;
  bins = 0;
}

//! Get the smoothing turns fraction
double Pulsar::MaskSmooth::get_turns () const
{
  return turns;
}

//! Set the fraction of masked neighbours required
void Pulsar::MaskSmooth::set_masked_fraction (double fraction)
{
  if (fraction <= 0.0 || fraction >= 1.0)
    throw Error (InvalidParam, "Pulsar::MaskSmooth::set_masked_fraction",
		 "invalid masked fraction = %lf", fraction);
  masked_fraction = fraction;
  masked_bins = 0;
}

//! Get the fraction of masked neighbours required
double Pulsar::MaskSmooth::get_masked_fraction () const
{
  return masked_fraction;
}


//
//
//

void Pulsar::MaskSmooth::set_bins (unsigned nbin)
{
  bins = nbin;
  turns = 0;
}

unsigned Pulsar::MaskSmooth::get_bins () const
{
  return bins;
}

void Pulsar::MaskSmooth::set_masked_bins (unsigned nbin)
{
  masked_bins = nbin;
  masked_bins = 0;
}

unsigned Pulsar::MaskSmooth::get_masked_bins () const
{
  return masked_bins;
}

//
//
//

void Pulsar::MaskSmooth::calculate (PhaseWeight* weight)
{
  unsigned nbin = input_weight->get_nbin();

  unsigned iwindow = bins;
  if (iwindow == 0)
    iwindow = nbin * turns;

  unsigned imasked = masked_bins;
  if (imasked == 0)
    imasked = iwindow * masked_fraction;

  if (imasked == 0)
    imasked = 1;

  unsigned ihalf = iwindow / 2;

#ifdef _DEBUG
  cerr << "Pulsar::MaskSmooth::calculate ihalf=" << ihalf 
       << " imasked=" << imasked << endl;
#endif

  for (unsigned ibin=0; ibin < nbin; ibin++)
  {
#ifdef _DEBUG
    cerr << ibin << " " << (*input_weight)[ibin] << ": ";
#endif

    (*weight)[ibin] = (*input_weight)[ibin];

    if ((*weight)[ibin] == mask_value)
    {
#ifdef _DEBUG
      cerr << "t" << endl;
#endif
      continue;
    }

    // count the masked bins before this bin
    unsigned nmasked = 0;
    for (unsigned jbin=0; jbin < ihalf; jbin++)
      if ((*input_weight)[ (ibin + jbin - ihalf + nbin) % nbin ] == mask_value)
	nmasked ++;

#ifdef _DEBUG
    cerr << "b:" << nmasked << " ";
#endif

    // count the masked bins after this bin
    for (unsigned jbin=0; jbin < ihalf; jbin++)
      if ((*input_weight)[ (ibin + jbin + 1 + nbin ) % nbin ] == mask_value)
	nmasked ++;

#ifdef _DEBUG
    cerr << "a:" << nmasked << " ";
#endif

    if (nmasked < imasked)
    {
#ifdef _DEBUG
      cerr << endl;
#endif
      continue;
    }

#ifdef _DEBUG
    cerr << "mask" << endl;
#endif
    (*weight)[ibin] = mask_value;

  }
}
