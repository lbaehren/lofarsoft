/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/StokesCylindrical.h"
#include "Pulsar/MultiPhase.h"
#include "Pulsar/StokesPlot.h"
#include "Pulsar/AnglePlot.h"

Pulsar::StokesCylindrical::Interface::Interface (StokesCylindrical* instance)
{
  if (instance)
    set_instance (instance);

  add( &StokesCylindrical::get_subint,
       &StokesCylindrical::set_subint,
       "subint", "Sub-integration to plot" );

  add( &StokesCylindrical::get_chan,
       &StokesCylindrical::set_chan,
       "chan", "Frequency channel to plot" );

  import ( MultiPhase::Interface() );

  // when this is set, import will filter out attributes with identical names
  import_filter = true;

  import ( "flux", StokesPlot::Interface(), &StokesCylindrical::get_flux );
  import ( "pa", AnglePlot::Interface(), &StokesCylindrical::get_orientation );
}
