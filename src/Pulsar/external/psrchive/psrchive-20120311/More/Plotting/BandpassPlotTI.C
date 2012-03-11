/***************************************************************************
 *
 *   Copyright (C) 2009 by Jonathan Khoo
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/BandpassPlot.h"

using namespace Pulsar;

BandpassPlot::Interface::Interface( BandpassPlot *instance )
{
  if (instance)
      set_instance (instance);

  add(&BandpassPlot::get_crop, &BandpassPlot::set_crop,
         "crop", "Crop max[min] fraction");
}
