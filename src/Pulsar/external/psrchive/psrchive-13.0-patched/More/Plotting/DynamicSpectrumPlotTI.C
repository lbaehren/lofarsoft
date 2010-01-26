/***************************************************************************
 *
 *   Copyright (C) 2009 by Paul Demorest
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/DynamicSpectrumPlot.h"
#include <pairutil.h>

Pulsar::DynamicSpectrumPlot::Interface::Interface (
    DynamicSpectrumPlot* instance)
{
  if (instance)
    set_instance (instance);

  add( &DynamicSpectrumPlot::get_srange,
      &DynamicSpectrumPlot::set_srange, 
      "srange", "Range of subints to plot" );

  add( &DynamicSpectrumPlot::get_pol,
      &DynamicSpectrumPlot::set_pol,
      "pol", "Polarization to plot" );

  import("cmap", pgplot::ColourMap::Interface(), 
      &DynamicSpectrumPlot::get_colour_map);

}
