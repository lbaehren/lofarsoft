/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
using namespace std;
#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Error.h"

/////////////////////////////////////////////////////////////////////////////
//
// Pulsar::Archive::total
//
/*!
  This method is primarily designed for use by the Archive::find_* methods.
*/
Pulsar::Archive* Pulsar::Archive::total (bool tscrunch) const try
{

  if (verbose == 3)
    cerr << "Pulsar::Archive::total extracting an empty copy" << endl;

  vector<unsigned> empty;
  Reference::To<Archive> copy = extract (empty);

  copy->set_nchan( 1 );
  copy->set_npol( 1 );
  copy->set_state( Signal::Intensity );

  if (verbose == 3)
    cerr << "Pulsar::Archive::total adding " << get_nsubint()
         << " Integration::total copies" << endl;

  for (unsigned isub=0; isub < get_nsubint(); isub++)
    copy->manage( get_Integration(isub)->total() );

  if (tscrunch)
    copy->tscrunch ();

  return copy.release();
}
catch (Error& error)
{
  throw error += "Pulsar::Archive::total";
}

