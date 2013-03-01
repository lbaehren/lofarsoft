/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/MultiPhase.h"
#include "Pulsar/PhaseScale.h"

Pulsar::MultiPhase::Interface::Interface (MultiPhase* instance)
{
  if (instance)
    set_instance (instance);

  import( "x", PhaseScale::Interface(), &MultiPhase::get_scale );
}
