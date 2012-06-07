/***************************************************************************
 *
 *   Copyright (C) 2005 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Profile.h"
#include "Pulsar/Smooth.h"
#include "Pulsar/BaselineWindow.h"

using namespace std;

/////////////////////////////////////////////////////////////////////////////
//
// Pulsar::Profile::find_min_phase
//
/*! Returns the centre phase of the region with minimum mean
  \param duty_cycle width of the region over which the mean is calculated
 */
float Pulsar::Profile::find_min_phase (float duty_cycle) const try {

  if (verbose)
    cerr << "Pulsar::Profile::find_min_phase" << endl;

  BaselineWindow mean;
  mean.get_smooth()->set_turns (duty_cycle);
  mean.set_find_minimum ();
  return mean.find_phase (get_nbin(), get_amps());
}
catch (Error& error) {
  throw error += "Pulsar::Profile::find_min_phase";
}


/////////////////////////////////////////////////////////////////////////////
//
// Pulsar::Profile::find_max_phase
//
/*! Returns the centre phase of the region with maximum mean
  \param duty_cycle width of the region over which the mean is calculated
 */
float Pulsar::Profile::find_max_phase (float duty_cycle) const try {

  if (verbose)
    cerr << "Pulsar::Profile::find_max_phase" << endl;
  
  BaselineWindow mean;
  mean.get_smooth()->set_turns (duty_cycle);
  mean.set_find_maximum ();
  return mean.find_phase (get_nbin(), get_amps());
}
catch (Error& error) {
  throw error += "Pulsar::Profile::find_max_phase";
}

