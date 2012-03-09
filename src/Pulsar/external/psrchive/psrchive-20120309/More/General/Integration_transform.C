/***************************************************************************
 *
 *   Copyright (C) 2003 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
using namespace std;
#include "Pulsar/Integration.h"
#include "Pulsar/PolnProfile.h"
#include "Pulsar/Profile.h"
#include "Error.h"

void Pulsar::Integration::transform (const Jones<float>& response)
{
  if (get_npol() != 4)
    throw Error (InvalidState, "Pulsar::Integration::transform",
		 "incomplete polarization information");

  Signal::Basis basis = get_basis();
  Signal::State state = get_state();

  for (unsigned ichan=0; ichan < get_nchan(); ichan++) try {

    PolnProfile poln (basis, state, 
		      profiles[0][ichan], profiles[1][ichan],
		      profiles[2][ichan], profiles[3][ichan]);

    poln.transform (response);

  }
  catch (Error& error) {
    if (verbose)
      cerr << "Pulsar::Integration::transform error ichan=" << ichan
           << error << endl;
    set_weight (ichan, 0);
  }
}
 
void Pulsar::Integration::transform (const vector< Jones<float> >& response)
{
  if (get_npol() != 4)
    throw Error (InvalidState, "Pulsar::Integration::transform",
		 "incomplete polarization information");

  if (response.size() != get_nchan())
    throw Error (InvalidState, "Pulsar::Integration::transform",
		 "response size=%d != nchan=%d", response.size(), get_nchan());

  if (verbose)
    cerr << "Pulsar::Integration::transform vector<Jones<float>>" << endl;

  Signal::Basis basis = get_basis();
  Signal::State state = get_state();

  for (unsigned ichan=0; ichan < get_nchan(); ichan++) try {

    PolnProfile poln (basis, state, 
		      profiles[0][ichan], profiles[1][ichan],
		      profiles[2][ichan], profiles[3][ichan]);

    poln.transform (response[ichan]);
    
  }
  catch (Error& error) {
    if (verbose)
      cerr << "Pulsar::Integration::transform error ichan=" << ichan
           << error << endl;
    set_weight (ichan, 0);
  }

}
 

