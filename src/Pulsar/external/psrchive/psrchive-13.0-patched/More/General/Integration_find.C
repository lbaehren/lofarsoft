/***************************************************************************
 *
 *   Copyright (C) 2002 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
using namespace std;
#include "Pulsar/Integration.h"
#include "Pulsar/Profile.h"
#include "Error.h"

/////////////////////////////////////////////////////////////////////////////
//
// Pulsar::Integration::find_transitions
//
/*!
  <UL>
  <LI> Calls Integration::total
  <LI> Calls Profile::find_transitions on the total intensity
  </UL>
*/
void Pulsar::Integration::find_transitions (int& hi2lo, int& lo2hi, int& buf)
  const try {

  if (verbose)
    cerr << "Pulsar::Integration::find_transitions call total" << endl;

  Reference::To<Integration> copy = total ();

  if (verbose)
    cerr << "Pulsar::Integration::find_transitions"
            " call Profile::find_transitions" << endl;

  copy->profiles[0][0]->find_transitions (hi2lo, lo2hi, buf);

}
catch (Error& err) {
  throw err += "Integration::find_transitions";
}

/////////////////////////////////////////////////////////////////////////////
//
// Pulsar::Integration::find_peak_edges
//
/*!
  <UL>
  <LI> Calls Integration::total
  <LI> Calls Profile::find_peak_edges on the total intensity
  </UL>
*/
void Pulsar::Integration::find_peak_edges (int& rise, int& fall) const
try {

  if (verbose)
    cerr << "Pulsar::Integration::find_peak_edges call total" << endl;

  Reference::To<Integration> copy = total ();

  if (verbose)
    cerr << "Pulsar::Integration::find_transitions"
            " call Profile::find_peak_edges" << endl;

  copy->profiles[0][0]->find_peak_edges (rise, fall);

}
catch (Error& err) {
  throw err += "Integration::find_peak_edges";
}


/////////////////////////////////////////////////////////////////////////////
//
// Pulsar::Integration::find_min_phase
//
/*!
  <UL>
  <LI> Calls Integration::total
  <LI> Calls Profile::find_min_phase on the total intensity
  </UL>
*/
float Pulsar::Integration::find_min_phase (float dc) const
try {

  if (Pulsar::Integration::verbose)
    cerr << "Pulsar::Integration::find_min_phase entered" << endl;

  Reference::To<Integration> copy = total ();
  return copy->profiles[0][0]->find_min_phase (dc);

}
catch (Error& err) {
  throw err += "Integration::find_peak_edges";
}


/////////////////////////////////////////////////////////////////////////////
//
// Pulsar::Integration::find_max_phase
//
/*!
  <UL>
  <LI> Calls Integration::total
  <LI> Calls Profile::find_max_phase on the total intensity
  </UL>
*/
float Pulsar::Integration::find_max_phase () const
try {

  Reference::To<Integration> copy = total ();
  return copy->profiles[0][0]->find_max_phase ();

}
catch (Error& err) {
  throw err += "Integration::find_peak_edges";
}

