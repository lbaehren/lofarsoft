/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/PhasePlot.h"
#include "Pulsar/PhaseScale.h"

Pulsar::PhasePlot::Interface::Interface (PhasePlot* instance)
{
  if (instance)
    set_instance (instance);

  import ( "x", PhaseScale::Interface(), &PhasePlot::get_scale );
}
