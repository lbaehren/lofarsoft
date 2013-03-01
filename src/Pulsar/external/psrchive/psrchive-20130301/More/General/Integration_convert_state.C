/***************************************************************************
 *
 *   Copyright (C) 2002 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Integration.h"
#include "Pulsar/PolnProfile.h"
#include "Pulsar/Profile.h"
#include "Error.h"

using namespace std;

/////////////////////////////////////////////////////////////////////////////
//
// Pulsar::Integration::convert_state
//
/*!

*/
void Pulsar::Integration::convert_state (Signal::State state) try
{
  if (verbose)
    cerr << "Pulsar::Integration::convert_state current=" 
         << Signal::state_string (get_state()) << " request="
         << Signal::state_string (state) << endl;

  if (get_state() == state)
    return;

  //////////////////////////////////////////////////////////////////////////
  // convert to Coherence products or Stokes parameters
  if (state == Signal::Coherence || state == Signal::Stokes) {
    poln_convert (state);
    return;
  }

  if (state == Signal::Invariant) {
    invint ();
    return;
  }

  if (state == Signal::Intensity) {
    pscrunch();
    return;
  }

  throw Error (InvalidParam, "Pulsar::Integration::convert_state",
	       "cannot convert from %s to %s", 
	       Signal::state_string (get_state()),
	       Signal::state_string (state));
}
catch (Error& error) {
  throw error += "Pulsar::Integration::convert_state";
}


//! Converts between coherency products and Stokes parameters
/*! As it performs no error checking, this method should not be called 
  directly.  Use Pulsar::Integration::convert_state. */
void Pulsar::Integration::poln_convert (Signal::State out_state)
{
  for (unsigned ichan=0; ichan < get_nchan(); ichan++) {

    PolnProfile poln (get_basis(), get_state(), 
		      get_Profile(0,ichan),get_Profile(1,ichan),
		      get_Profile(2,ichan),get_Profile(3,ichan));
    poln.convert_state (out_state);

  }
}





