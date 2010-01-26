/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/PhaseVsHistPlus.h"
#include "Pulsar/PhaseVsHist.h"
#include "Pulsar/StokesPlot.h"
#include "Pulsar/PhaseScale.h"

Pulsar::PhaseVsHistPlus::Interface::Interface
(PhaseVsHistPlus* instance)
{
  if (instance)
    set_instance (instance);

  import( "x", PhaseScale::Interface(), &PhaseVsHistPlus::get_scale );

  // when this is set, import will filter out attributes with identical names
  import_filter = true;

  import ( "hist", PhaseVsHist::Interface(), &PhaseVsHistPlus::get_hist );
  import ( "flux", StokesPlot::Interface(), &PhaseVsHistPlus::get_flux );
}
