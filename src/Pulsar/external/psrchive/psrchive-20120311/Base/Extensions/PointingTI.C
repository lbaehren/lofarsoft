/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/



#include "Pulsar/Pointing.h"



using Pulsar::Pointing;



Pointing::Interface::Interface ( Pointing *s_instance )
{
  if( s_instance )
    set_instance( s_instance );

  add( &Pointing::get_local_sidereal_time,
         &Pointing::set_local_sidereal_time,
         "lst", "Local sidereal time (seconds)" );

  add( &Pointing::get_galactic_longitude,
         &Pointing::set_galactic_longitude,
         "gb", "Galactic longitude (deg)" );

  add( &Pointing::get_galactic_latitude,
         &Pointing::set_galactic_latitude,
         "gl", "Galactic latitude (deg)" );

  add( &Pointing::get_feed_angle,
         &Pointing::set_feed_angle,
         "fa", "Feed angle (deg)" );

  add( &Pointing::get_position_angle,
         &Pointing::set_position_angle,
         "pa", "Position angle of the feed (deg)" );

  add( &Pointing::get_parallactic_angle,
         &Pointing::set_parallactic_angle,
         "va", "Vertical (parallactic) angle (deg)" );

  add( &Pointing::get_telescope_azimuth,
         &Pointing::set_telescope_azimuth,
         "az", "Telescope azimuth" );

  add( &Pointing::get_telescope_zenith,
         &Pointing::set_telescope_zenith,
         "zen", "Telescope zenith" );
}




