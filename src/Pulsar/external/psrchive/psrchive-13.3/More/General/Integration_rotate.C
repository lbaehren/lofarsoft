/***************************************************************************
 *
 *   Copyright (C) 2002 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
using namespace std;
#include "Pulsar/Integration.h"
#include "Pulsar/Profile.h"

void Pulsar::Integration::rotate (double time)
{
  double pfold = get_folding_period ();

  if (pfold <= 0.0)
    throw Error (InvalidParam, "Integration::rotate",
                 "folding period=%lf", pfold);

  try {
    rotate_phase (time/pfold);
    set_epoch (get_epoch() + time);
  }
  catch (Error& error) {
    throw error += "Integration::rotate";
  }
}

void Pulsar::Integration::rotate_phase (double phase) try {

  // only Archive::apply_model guarantees preservation of polyco phase
  zero_phase_aligned = false;

  for (unsigned ipol=0; ipol<get_npol(); ipol++)
    for (unsigned ichan=0; ichan<get_nchan(); ichan++)
      profiles[ipol][ichan] -> rotate_phase (phase);

}
catch (Error& error) {
  throw error += "Integration::rotate_phase";
}

