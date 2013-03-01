/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
using namespace std;
#include "Pulsar/Integration.h"
#include "Error.h"

/////////////////////////////////////////////////////////////////////////////
//
// Pulsar::Integration::total
//
/*!
  This method is primarily designed for use by the Integration::find_*
  methods.  After calling fscrunch, the resulting Profile may have a 
  different centre frequency than that of the Integration.  Therefore,
  the Profile is dedispersed to match the phase referenced at the frequency
  returned by Integration::get_centre_frequency.
*/
Pulsar::Integration* Pulsar::Integration::total () const
{
  if (verbose)
    cerr << "Pulsar::Integration::total entered" << endl;

  if (get_npol()<1 || get_nchan()<1)
    throw Error (InvalidState, "Pulsar::Integration::total",
                 "npol=%d nchan=%d", get_npol(), get_nchan());

  try {

    Reference::To<Integration> copy = clone ();
    copy->orphan ();

    copy->pscrunch ();

    if (verbose)
      cerr << "Pulsar::Integration::total dedisperse" << endl;

    copy->dedisperse();

    if (verbose)
      cerr << "Pulsar::Integration::total fscrunch" << endl;

    copy->fscrunch ();
    return copy.release();

  }
  catch (Error& err) {
    throw err += "Integration::total";
  }
}

