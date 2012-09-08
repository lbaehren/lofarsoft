/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/TwoBitPlot.h"

Pulsar::TwoBitPlot::Interface::Interface (TwoBitPlot* instance)
{
  if (instance)
    set_instance (instance);

}
