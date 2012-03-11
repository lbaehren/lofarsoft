/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/PhaseVsFrequencyPlus.h"
#include "Pulsar/PhaseVsFrequency.h"
#include "Pulsar/SpectrumPlot.h"
#include "Pulsar/ProfilePlot.h"
#include "Pulsar/PhaseScale.h"

Pulsar::PhaseVsFrequencyPlus::Interface::Interface
(PhaseVsFrequencyPlus* instance)
{
  if (instance)
    set_instance (instance);

  add( &PhaseVsFrequencyPlus::get_subint,
       &PhaseVsFrequencyPlus::set_subint,
       "subint", "Sub-integration to plot" );

  add( &PhaseVsFrequencyPlus::get_pol,
       &PhaseVsFrequencyPlus::set_pol,
       "pol", "Polarization to plot" );

  add( &PhaseVsFrequencyPlus::get_plot_psd,
       &PhaseVsFrequencyPlus::set_plot_psd,
       "psd", "Plot the power spectral density" );

  import( "x", PhaseScale::Interface(), &PhaseVsFrequencyPlus::get_scale );

  // when this is set, import will filter out attributes with identical names
  import_filter = true;

  import ( "freq", PhaseVsFrequency::Interface(), &PhaseVsFrequencyPlus::get_freq );
  import ( "flux", ProfilePlot::Interface(),      &PhaseVsFrequencyPlus::get_flux );
  import ( "psd",  SpectrumPlot::Interface(),     &PhaseVsFrequencyPlus::get_psd );
}
