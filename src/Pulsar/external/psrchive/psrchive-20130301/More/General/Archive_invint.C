/***************************************************************************
 *
 *   Copyright (C) 2002 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
using namespace std;
#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"

void Pulsar::Archive::invint () try {

  if (get_nsubint() == 0)
    return;
  
  remove_baseline();
  
  for (unsigned isub=0; isub < get_nsubint(); isub++)
    get_Integration(isub) -> invint ();
  
  set_state( Signal::Invariant );
  set_npol(1);
}
catch (Error& error) {
  throw error += "Pulsar::Archive::invint";
}
