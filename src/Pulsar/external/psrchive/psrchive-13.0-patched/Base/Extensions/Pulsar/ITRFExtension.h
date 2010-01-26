//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2003 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Base/Extensions/Pulsar/ITRFExtension.h,v $
   $Revision: 1.12 $
   $Date: 2008/05/26 20:51:52 $
   $Author: straten $ */

#ifndef __ITRFExtension_h
#define __ITRFExtension_h

#include "Pulsar/ArchiveExtension.h"

namespace Pulsar {
  
  //! International Terrestrial Reference Frame Extension
  /*! This ITRFExtension class provides a container for
    holding the ITRF coordinates of a telescope. */

  class ITRFExtension : public Pulsar::Archive::Extension {
    
  public:
    
    //! Default constructor
    ITRFExtension ();
    
    //! Copy constructor
    ITRFExtension (const ITRFExtension& extension);
    
    //! Operator =
    const ITRFExtension& operator= (const ITRFExtension& extension);
    
    //! Destructor
    ~ITRFExtension ();
    
    //! Clone method
    ITRFExtension* clone () const { return new ITRFExtension( *this ); }
    
    //! Return a text interfaces that can be used to access this instance
    TextInterface::Parser* get_interface();
    class Interface;
    
    //! Return an abbreviated name that can be typed relatively quickly
    std::string get_short_name () const { return "itrf"; }

    // get methods
    double get_ant_x() const { return ant_x; }
    double get_ant_y() const { return ant_y; }
    double get_ant_z() const { return ant_z; }

    //! Get geocentric latitude and longitude in radians, and radius in metres
    void get_sph (double& lat, double& lon, double& rad);

    ////////////////////////////////////////////////////////////////

    //! Antenna ITRF X-coordinate
    double ant_x;
    
    //! Antenna ITRF Y-coordinate
    double ant_y;
    
    //! Antenna ITRF Z-coordinate
    double ant_z;

  };
 

}

#endif
