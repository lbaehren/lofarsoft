/***************************************************************************
 *
 *   Copyright (C) 2003 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/ITRFExtension.h"
#include "TextInterface.h"

//! Default constructor
Pulsar::ITRFExtension::ITRFExtension ()
  : Extension ("ITRFExtension")
{
  ant_x = 0.0;
  ant_y = 0.0;
  ant_z = 0.0;
}

//! Copy constructor
Pulsar::ITRFExtension::ITRFExtension (const ITRFExtension& extension)
  : Extension ("ITRFExtension")
{
  ant_x = extension.ant_x;
  ant_y = extension.ant_y;
  ant_z = extension.ant_z;
}

//! Operator =
const Pulsar::ITRFExtension&
Pulsar::ITRFExtension::operator= (const ITRFExtension& extension)
{
  ant_x = extension.ant_x;
  ant_y = extension.ant_y;
  ant_z = extension.ant_z;

  return *this;
}

void
Pulsar::ITRFExtension::get_sph (double& lat, double& lon, double& rad)
{
  rad = sqrt( ant_x*ant_x + ant_y*ant_y + ant_z*ant_z );
  lat = asin (ant_z/rad);
  lon = atan2 (ant_y, ant_x);
}

//! Destructor
Pulsar::ITRFExtension::~ITRFExtension ()
{
}

// Text interface to a ITRFExtension
class Pulsar::ITRFExtension::Interface
  : public Extension::Interface<ITRFExtension>
{
public:
  Interface( ITRFExtension *s_instance = NULL )
  {
    if( s_instance )
      set_instance( s_instance );

    add( &ITRFExtension::get_ant_x,
         &ITRFExtension::set_ant_x,
        "ant_x", "ITRF X coordinate." );

    add( &ITRFExtension::get_ant_y,
         &ITRFExtension::set_ant_y,
         "ant_y", "ITRF Y coordinate." );

    add( &ITRFExtension::get_ant_z,
         &ITRFExtension::set_ant_z,
        "ant_z", "ITRF Z coordinate." );
  }
};

//! Return a text interfaces that can be used to access this instance
TextInterface::Parser* Pulsar::ITRFExtension::get_interface()
{
  return new Interface( this );
}
