/***************************************************************************
 *
 *   Copyright (C) 2010 by Jonathan
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/StokesCylindricalPlus.h"
#include "Pulsar/AnglePlot.h"
#include "Pulsar/MultiPhase.h"
#include "Pulsar/StokesPlot.h"

Pulsar::StokesCylindricalPlus::Interface::Interface (StokesCylindricalPlus* instance)
{
  if (instance)
    set_instance (instance);

  add( &StokesCylindricalPlus::get_subint,
       &StokesCylindricalPlus::set_subint,
       "subint", "Sub-integration to plot" );

  add( &StokesCylindricalPlus::get_chan,
       &StokesCylindricalPlus::set_chan,
       "chan", "Frequency channel to plot" );

  add( &StokesCylindricalPlus::get_flux_crop,
       &StokesCylindricalPlus::set_flux_crop,
       "crop", "Value to crop for flux plot" );

  import ( MultiPhase::Interface() );

  // when this is set, import will filter out attributes with identical names
  import_filter = true;

  import ( "flux", StokesPlot::Interface(), &StokesCylindricalPlus::get_flux );
  import ( "flux_cropped", StokesPlot::Interface(), &StokesCylindricalPlus::get_flux_cropped );
  import ( "pa", AnglePlot::Interface(), &StokesCylindricalPlus::get_orientation );
}
