/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/Poincare.h"

Pulsar::Poincare::Interface::Interface (Poincare* instance)
{
  if (instance)
    set_instance (instance);

  add( &Poincare::get_subint,
       &Poincare::set_subint,
       "subint", "Sub-integration to plot" );

  add( &Poincare::get_chan,
       &Poincare::set_chan,
       "chan", "Frequency channel to plot" );

  import( "ph", PlotScale::Interface(), &Poincare::get_phase_scale );

  add( &Poincare::get_longitude,
       &Poincare::set_longitude,
       "long", "Camera longitude" );

  add( &Poincare::get_latitude,
       &Poincare::set_latitude,
       "lat", "Camera latitude" );

  add( &Poincare::get_nstep,
       &Poincare::set_nstep,
       "nstep", "Number of animation steps" );

  add( &Poincare::get_longitude_step,
       &Poincare::set_longitude_step,
       "dlong", "Change in longitude on each step" );

  add( &Poincare::get_latitude_step,
       &Poincare::set_latitude_step,
       "dlat", "Change in latitude on each step" );

  add( &Poincare::get_label_bins,
       &Poincare::set_label_bins,
       "bins", "Phase bins to be labeled" );
}
