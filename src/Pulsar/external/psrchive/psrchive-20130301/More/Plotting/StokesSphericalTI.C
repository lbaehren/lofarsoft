/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/StokesSpherical.h"
#include "Pulsar/MultiPhase.h"
#include "Pulsar/StokesPlot.h"
#include "Pulsar/AnglePlot.h"

Pulsar::StokesSpherical::Interface::Interface (StokesSpherical* instance)
{
  if (instance)
    set_instance (instance);

  add( &StokesSpherical::get_subint,
       &StokesSpherical::set_subint,
       "subint", "Sub-integration to plot" );

  add( &StokesSpherical::get_chan,
       &StokesSpherical::set_chan,
       "chan", "Frequency channel to plot" );

  import ( MultiPhase::Interface() );

  // when this is set, import will filter out attributes with identical names
  import_filter = true;

  import ( "flux", StokesPlot::Interface(), &StokesSpherical::get_flux );
  import ( "pa",  AnglePlot::Interface(), &StokesSpherical::get_orientation );
  import ( "ell", AnglePlot::Interface(), &StokesSpherical::get_ellipticity );
}
