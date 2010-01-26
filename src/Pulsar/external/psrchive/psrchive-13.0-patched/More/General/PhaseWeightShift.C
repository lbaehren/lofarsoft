/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/PhaseWeightShift.h"
#include "Pulsar/PhaseWeight.h"
#include "templates.h"

#include <string.h>

#include <algorithm>

using namespace std;

static double no_shift ()
{
  throw Error (InvalidState, "no_shift",
	       "Pulsar::PhaseWeightShift::get_shift not initialized");
}

Pulsar::PhaseWeightShift::PhaseWeightShift ()
  : get_shift( no_shift )
{

}

Pulsar::PhaseWeightShift* Pulsar::PhaseWeightShift::clone () const
{
  return new PhaseWeightShift (*this);
}

//! Shift the input PhaseWeight array into the result
void Pulsar::PhaseWeightShift::calculate (PhaseWeight* result)
{
  unsigned nbin = input_weight->get_nbin();

  double phase = - get_shift ();

  // Ensure that phase runs from 0 to 1.
  phase -= floor (phase);

  // Round to the nearest integer number of phase bins
  unsigned binshift = unsigned (phase * double(nbin) + 0.5);

#ifdef _DEBUG
  cerr << "Pulsar::PhaseWeightShift::calculate nbin input=" << nbin
       << " output=" << result->get_nbin() << " binshift=" << binshift << endl;
#endif

  // Shift the input PhaseWeight bins
  shift (nbin, binshift, result->get_weights(), input_weight->get_weights());
}
