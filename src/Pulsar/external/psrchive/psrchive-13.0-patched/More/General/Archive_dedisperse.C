/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
using namespace std;

#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"

/*!
  The dedisperse method removes the dispersive delay between
  each frequency channel and that of the reference frequency
  defined by get_centre_frequency.
*/
void Pulsar::Archive::dedisperse () try {

  if (get_nsubint() == 0)
    return;

  for (unsigned isub=0; isub < get_nsubint(); isub++)
    get_Integration(isub) -> dedisperse ();

  set_dedispersed (true);

}
catch (Error& error) {
  throw error += "Pulsar::Archive::dedisperse";
}

