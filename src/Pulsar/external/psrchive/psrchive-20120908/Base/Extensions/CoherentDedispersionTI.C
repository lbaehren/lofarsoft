/***************************************************************************
 *
 *   Copyright (C) 2010 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/CoherentDedispersion.h"

using Pulsar::CoherentDedispersion;

CoherentDedispersion::Interface::Interface( CoherentDedispersion *s_instance )
{
  if (s_instance)
    set_instance (s_instance);

  add( &CoherentDedispersion::get_dispersion_measure,
       "dm", "Dispersion measure" );

  add( &CoherentDedispersion::get_doppler_correction,
       "doppler", "Doppler correction" );

  add( &CoherentDedispersion::get_nbit_data,
       "nbit", "Number of bits in data" );

  add( &CoherentDedispersion::get_nbit_chirp,
       "nbit", "Number of bits in filter" );

  add( &CoherentDedispersion::get_nchan_input,
       "nchan", "Number of input frequency channels" );

#if 0
  import( "int", IntegrationTI(),
          (Integration*(Archive::*)(unsigned)) &Archive::get_Integration,
          &Archive::get_nsubint );
#endif
}
