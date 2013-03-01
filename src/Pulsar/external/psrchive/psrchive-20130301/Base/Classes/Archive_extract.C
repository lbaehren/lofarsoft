/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Error.h"

using namespace std;

/////////////////////////////////////////////////////////////////////////////
//
// Pulsar::Archive::extract
//
Pulsar::Archive* Pulsar::Archive::extract (vector<unsigned>& subs) const
{
  if (verbose > 2)
    cerr << "Pulsar::Archive::extract extracting " << subs.size() << " subints"
	 << endl;

  Reference::To<Archive> copy;

  copy_subints = &subs;
  try
  {
    copy = clone ();
  }
  catch (Error& error)
  {
    copy_subints = 0;
    throw error += " Pulsar::Archive::extract";
  }

  copy_subints = 0;

  return copy.release();
}

unsigned Pulsar::Archive::copy_nsubint () const
{
  if (copy_subints)
    return copy_subints->size();
  else
    return get_nsubint();
}

unsigned Pulsar::Archive::copy_isubint (unsigned isub) const
{
  if (!copy_subints)
    return isub;

  if (isub >= copy_subints->size())
    throw Error (InvalidParam, "Pulsar::Archive::copy_isubint",
                 "isub=%u >= copy_nsubint=%u", isub, copy_subints->size());

  return (*copy_subints)[isub];
}

