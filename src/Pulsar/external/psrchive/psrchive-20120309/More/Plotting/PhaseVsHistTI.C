/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/PhaseVsHist.h"
#include "Pulsar/PhaseVsPlot.h"

Pulsar::PhaseVsHist::Interface::Interface (PhaseVsHist* instance)
{
  if (instance)
    set_instance (instance);

  add( &PhaseVsHist::get_chan,
       &PhaseVsHist::set_chan,
       "chan", "Frequency channel to plot" );

  add( &PhaseVsHist::get_weight,
       &PhaseVsHist::set_weight,
       "weight", "Weighting scheme (none, flux, snr)" );

  add( &PhaseVsHist::get_kernel,
       &PhaseVsHist::set_kernel,
       "kernel", "Histogram kernel (mises, delta)" );

  // import the interface of the base class
  import( PhaseVsPlot::Interface() );

}
