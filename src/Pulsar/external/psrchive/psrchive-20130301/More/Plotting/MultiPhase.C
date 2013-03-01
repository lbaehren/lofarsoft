/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/MultiPhase.h"

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

// Text interface to the MultiPhase class
Pulsar::MultiPhase::Interface::Interface (MultiPhase* instance)
{
  if (instance)
    set_instance (instance);
  
  import( "x", PhaseScale::Interface(), &MultiPhase::get_scale );
}

//! Get the text interface to the configuration attributes
TextInterface::Parser* Pulsar::MultiPhase::get_interface ()
{
  return new Interface (this);
}
