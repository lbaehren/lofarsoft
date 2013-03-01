/***************************************************************************
 *
 *   Copyright (C) 2002-2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Archive.h"
#include "Pulsar/TimeIntegrate.h"

using namespace std;

/*!
  \param nscrunch number of neighbouring Integrations to add. 
                  If nscrunch == 0, then add all Integrations together
 */
void Pulsar::Archive::tscrunch (unsigned nscrunch)
{
  TimeIntegrate operation;
  TimeIntegrate::EvenlySpaced policy;

  operation.set_range_policy( &policy );
  policy.set_nintegrate (nscrunch);
  operation.transform (this);
}

/////////////////////////////////////////////////////////////////////////////
//
/*!
  Useful wrapper for Archive::tscrunch
*/
void Pulsar::Archive::tscrunch_to_nsub (unsigned new_nsub)
{
  if (new_nsub == 0)
    throw Error (InvalidParam, "Pulsar::Archive::tscrunch_to_nsub",
		 "new nsub == 0");

  if (get_nsubint() < new_nsub)
    throw Error (InvalidParam, "Pulsar::Archive::tscrunch_to_nsub",
		 "new nsub=%u > old nsub=%u", new_nsub,get_nsubint());

  unsigned factor = get_nsubint() / new_nsub;
  if (get_nsubint() % new_nsub)
    factor ++;

  tscrunch (factor);
}
