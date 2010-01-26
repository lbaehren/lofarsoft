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

//! Returns a pointer to a new PolnProfile instance specified by ichan
Pulsar::PolnProfile* Pulsar::Integration::new_PolnProfile (unsigned ichan)
{
  const Integration* thiz = this;
  return const_cast<PolnProfile*>( thiz->new_PolnProfile(ichan) );
}

const Pulsar::PolnProfile*
Pulsar::Integration::new_PolnProfile (unsigned ichan) const
{
  if (verbose)
    cerr << "Pulsar::Integration::new_PolnProfile" << endl;

  if (get_npol() != 4)
    throw Error (InvalidState, "Pulsar::Integration::new_PolnProfile",
		 "incomplete polarization information");

  if (ichan >= get_nchan())
    throw Error (InvalidParam, "Pulsar::Integration::new_PolnProfile",
		 "ichan=%d >= nchan=%d", ichan, get_nchan());

  return new PolnProfile (get_basis(), get_state(), 
			  profiles[0][ichan], profiles[1][ichan],
			  profiles[2][ichan], profiles[3][ichan]);
}
