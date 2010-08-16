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
  Simply calls Integration::pscrunch for each Integration
*/
void Pulsar::Archive::pscrunch()
{
  if (get_nsubint() == 0)
    return;

  for (unsigned isub=0; isub < get_nsubint(); isub++)
    get_Integration(isub) -> pscrunch ();

  set_npol( 1 );

  set_state( Signal::pscrunch (get_state()) );
}

