/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/MultiPhase.h"

using namespace std;

Pulsar::MultiPhase::MultiPhase ()
{
  frames.set_shared_x_scale (new PhaseScale);
}

//! Get the scale
Pulsar::PhaseScale* Pulsar::MultiPhase::get_scale ()
{
  PhaseScale* scale = dynamic_cast<PhaseScale*>( frames.get_shared_x_scale() );
  if (!scale)
    throw Error (InvalidState, "Pulsar::MultiPhase::get_scale",
		 "x scale is not a PhaseScale");
  return scale;
}
