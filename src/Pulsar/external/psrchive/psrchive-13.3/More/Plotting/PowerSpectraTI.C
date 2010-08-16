/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/PowerSpectra.h"

Pulsar::PowerSpectra::Interface::Interface (PowerSpectra* instance)
{
  if (instance)
    set_instance (instance);

  add( &PowerSpectra::get_subint,
       &PowerSpectra::set_subint,
       "subint", "Sub-integration to plot" );

  add( &PowerSpectra::get_pol,
       &PowerSpectra::set_pol,
       "pol", "Polarization to plot" );

  add( &PowerSpectra::get_median_window,
       &PowerSpectra::set_median_window,
       "med", "Median smoothing window size" );

  add( &PowerSpectra::get_draw_lines,
       &PowerSpectra::set_draw_lines,
       "lines", "Connect points with lines" );
}
