/***************************************************************************
 *
 *   Copyright (C) 2002 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
using namespace std;
#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/Profile.h"
#include "Error.h"

/////////////////////////////////////////////////////////////////////////////
//
// Pulsar::Archive::find_transitions
//
/*!
  <UL>
  <LI> Calls Archive::total
  <LI> Calls Profile::find_transitions on the total intensity
  </UL>
*/
void Pulsar::Archive::find_transitions (int& hi2lo, int& lo2hi, int& buf)
  const
{
  Reference::To<Archive> copy = total ();
  copy->get_Profile(0,0,0)->find_transitions (hi2lo, lo2hi, buf);
}

/////////////////////////////////////////////////////////////////////////////
//
// Pulsar::Archive::find_peak_edges
//
/*!
  <UL>
  <LI> Calls Archive::total
  <LI> Calls Profile::find_peak_edges on the total intensity
  </UL>
*/
void Pulsar::Archive::find_peak_edges (int& rise, int& fall) const
{
  Reference::To<Archive> copy = total ();
  copy->get_Profile(0,0,0)->find_peak_edges (rise, fall);
}

/////////////////////////////////////////////////////////////////////////////
//
// Pulsar::Archive::find_min_phase
//
/*!
  <UL>
  <LI> Calls Archive::total
  <LI> Calls Profile::find_min_phase on the total intensity
  </UL>
*/
float Pulsar::Archive::find_min_phase (float dc) const
{
  Reference::To<Archive> copy = total ();
  return copy->get_Profile(0,0,0)->find_min_phase (dc);
}

/////////////////////////////////////////////////////////////////////////////
//
// Pulsar::Archive::find_max_phase
//
/*!
  <UL>
  <LI> Calls Archive::total
  <LI> Calls Profile::find_max_phase on the total intensity
  </UL>
*/
float Pulsar::Archive::find_max_phase () const
{
  Reference::To<Archive> copy = total ();
  return copy->get_Profile(0,0,0)->find_max_phase ();
}
