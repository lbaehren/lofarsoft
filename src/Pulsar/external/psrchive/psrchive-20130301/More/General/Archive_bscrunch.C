/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"

using namespace std;

/*!
  Useful wrapper for Archive::bscrunch
*/
void Pulsar::Archive::bscrunch_to_nbin (unsigned new_nbin)
{
  if (get_nsubint() == 0)
    return;

  for (unsigned isub=0; isub < get_nsubint(); isub++)
    get_Integration(isub) -> bscrunch_to_nbin (new_nbin);

  set_nbin (get_Integration(0)->get_nbin());
}

/*!
  Simply calls Integration::bscrunch for each Integration
  \param nscrunch the number of phase bins to add together
  */
void Pulsar::Archive::bscrunch (unsigned nscrunch)
{
  if (get_nsubint() == 0)
    return;

  for (unsigned isub=0; isub < get_nsubint(); isub++)
    get_Integration(isub) -> bscrunch (nscrunch);

  set_nbin (get_Integration(0)->get_nbin());
}


