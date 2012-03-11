/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/Profile.h"

using namespace std;

/*! Rotate pulsar Integrations so that the bin of largest amplitude
    is centred */
void Pulsar::Archive::centre_max_bin (double phase_offset)
{
  Reference::To<Pulsar::Archive> arch = total();
  double period = arch->get_Integration(0)->get_folding_period();

  int max_bin = arch->get_Profile(0,0,0)->find_max_bin();

  double shift_phase = double(max_bin) / double(get_nbin()) - phase_offset;
  double shift_time = shift_phase * period;

  rotate (shift_time); 
}
