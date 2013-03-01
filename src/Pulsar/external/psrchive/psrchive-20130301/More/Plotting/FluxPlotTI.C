/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/FluxPlot.h"
#include "Pulsar/PhasePlot.h"

Pulsar::FluxPlot::Interface::Interface (FluxPlot* instance)
{
  if (instance)
    set_instance (instance);

  add( &FluxPlot::get_subint,
       &FluxPlot::set_subint,
       "subint", "Sub-integration to plot" );

  add( &FluxPlot::get_chan,
       &FluxPlot::set_chan,
       "chan", "Frequency channel to plot" );

  add( &FluxPlot::get_pol,
       &FluxPlot::set_pol,
       "pol", "Polarization to plot" );

  add( &FluxPlot::get_logarithmic,
       &FluxPlot::set_logarithmic,
       "log", "Logarithmic scale" );

  import( PhasePlot::Interface() );

  add( &FluxPlot::get_peak_zoom,
       &FluxPlot::set_peak_zoom,
       "peak", "Auto zoom on peak" );

  add( &FluxPlot::get_baseline_zoom,
       &FluxPlot::set_baseline_zoom,
       "base", "Auto zoom on baseline" );

  add( &FluxPlot::get_info_label,
       &FluxPlot::set_info_label,
       "info", "Use informative label" );

  add( &FluxPlot::get_plot_error_box,
       &FluxPlot::set_plot_error_box,
       "ebox", "Plot error box" );

  add( &FluxPlot::get_plot_histogram,
       &FluxPlot::set_plot_histogram,
       "hist", "Plot histogram style" );
  
  add( &FluxPlot::get_crop,
       &FluxPlot::set_crop,
       "crop", "Crop max[min] fraction" );
}
