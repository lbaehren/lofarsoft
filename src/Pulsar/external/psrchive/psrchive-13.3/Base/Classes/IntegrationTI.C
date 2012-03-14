/***************************************************************************
 *
 *   Copyright (C) 2004-2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/IntegrationTI.h"
#include "Pulsar/Pointing.h"

Pulsar::IntegrationTI::IntegrationTI ()
{
  add( &Integration::get_nchan, "nchan", "Number of frequency channels" );
  add( &Integration::get_npol,  "npol",  "Number of polarizations" );
  add( &Integration::get_nbin,  "nbin",  "Number of pulse phase bins" );

  add( &Integration::get_epoch,
       &Integration::set_epoch,
       "mjd", "Epoch (MJD)" );
 
  add( &Integration::get_duration,
       &Integration::set_duration,
       "duration", "Integration length (s)" );

  add( &Integration::get_folding_period,
       &Integration::set_folding_period, 
       "period", "Folding period (s)" );

  // note that explicit casts are required for overloaded methods

  VGenerator<double> dgenerator;
  add_value(dgenerator( "freq", "Channel centre frequency (MHz)",
			( double (Integration::*) (unsigned) const )
			&Integration::get_centre_frequency,
			&Integration::set_centre_frequency,
			&Integration::get_nchan ));

  VGenerator<float> fgenerator;
  add_value(fgenerator( "wt", "Channel weight",
			&Integration::get_weight,
			&Integration::set_weight,
			&Integration::get_nchan ));

  import( "point", Pulsar::Pointing::Interface(),
	  ( Pointing* (Integration::*) () ) &Integration::get<Pointing> );
}

