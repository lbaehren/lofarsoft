/***************************************************************************
 *
 *   Copyright (C) 2003-2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Error.h"

using namespace std;

//! Perform the transformation on each polarimetric profile
void Pulsar::Archive::transform (const Jones<float>& jones) try {

  unsigned nsub = get_nsubint ();

  for (unsigned isub=0; isub < nsub; isub++)
    get_Integration (isub) -> transform (jones);

}
catch (Error& error)
{
  throw error += "Pulsar::Archive::transform (Jones<float>)";
}


//! Perform frequency response on each polarimetric profile
void Pulsar::Archive::transform (const vector<Jones<float> >& jones) try {

  unsigned nsub = get_nsubint ();

  for (unsigned isub=0; isub < nsub; isub++)
    get_Integration (isub) -> transform (jones);

}
catch (Error& error)
{
  throw error += "Pulsar::Archive::transform (vector<Jones<float>>)";
}

//! Perform the time and frequency response on each polarimetric profile
void Pulsar::Archive::transform (const vector<vector<Jones<float> > >& J) try
{
  unsigned nsub = get_nsubint ();
  
  if (J.size() != nsub)
    throw Error (InvalidState, "Pulsar::Archive::transform",
		 "vector of vector of Jones matrices size=%u != nsubint=%u",
		 J.size(), nsub);

  for (unsigned isub=0; isub < nsub; isub++)
    get_Integration (isub) -> transform (J[isub]);
  
}
catch (Error& error)
{
  throw error += "Pulsar::Archive::transform (vector<vector<Jones<float>>>)";
}


