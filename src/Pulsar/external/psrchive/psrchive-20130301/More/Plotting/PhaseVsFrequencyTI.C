/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/PhaseVsFrequency.h"
#include "Pulsar/PhaseVsPlot.h"

Pulsar::PhaseVsFrequency::Interface::Interface (PhaseVsFrequency* instance)
{
  if (instance)
    set_instance (instance);

  add( &PhaseVsFrequency::get_subint,
       &PhaseVsFrequency::set_subint,
       "subint", "Sub-integration to plot" );

  add( &PhaseVsFrequency::get_pol,
       &PhaseVsFrequency::set_pol,
       "pol", "Polarization to plot" );

  // import the interface of the base class
  import( PhaseVsPlot::Interface() );



}
