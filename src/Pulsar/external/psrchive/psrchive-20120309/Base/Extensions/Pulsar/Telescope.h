//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Base/Extensions/Pulsar/Telescope.h,v $
   $Revision: 1.11 $
   $Date: 2008/11/21 19:03:52 $
   $Author: demorest $ */

#ifndef __Telescope_Extension_h
#define __Telescope_Extension_h

#include "Pulsar/ArchiveExtension.h"
#include "Directional.h"

namespace Pulsar
{
  
  //! Contains information about the telescope used for the observation
  class Telescope : public Pulsar::Archive::Extension
  {

  public:
    
    //! Default constructor
    Telescope ();

    //! Copy constructor
    Telescope (const Telescope& extension);

    //! Operator =
    const Telescope& operator= (const Telescope& extension);

    //! Destructor
    ~Telescope ();

    //! Clone method
    Telescope* clone () const { return new Telescope(*this); }

    //////////////////////////////////////////////////////////////////////
    //
    // Information about the primary reflector
    //

    //! Types of primary reflector mounts
    enum Mount
    {
      //! Equatorial (right ascension, declination)
      Equatorial,
      //! Horizon (altidue, azimuth)
      Horizon,
      //! Meridian (aka X-Y, meridianal longitude and latitude)
      Meridian,
      //! Fixed (LOFAR, Arecibo)
      Fixed,
      //! Mobile (satellite)
      Mobile
    };

    //! Types of primary reflector
    enum Reflector
    {
      //! Parabolic
      Parabolic,
      //! Spherical
      Spherical
    };

    //! Types of focus
    enum Focus 
    {
      /*! Prime Focus: primary reflector; receiver at primary focus */
      PrimeFocus,
      /*! Cassegrain: primary and secondary reflectors; receiver at
	secondary focus; prime focus is behind secondary reflector */
      Cassegrain,
      /*! Gregorian: primary and secondary reflectors; receiver at
	secondary focus; prime focus is in front of secondary reflector */
      Gregorian
    };

    //! Get the name of the observatory and/or primary reflector
    std::string get_name() const { return name; }
    //! Set the name of the observatory and/or primary reflector
    void set_name (const std::string& _name) { name = _name; }

    //! Get the latitude of the antenna
    Angle get_latitude() const { return latitude; }
    //! Set the latitude of the antenna
    void set_latitude (const Angle& _latitude) { latitude = _latitude; }

    //! Get the longitude of the antenna
    Angle get_longitude() const { return longitude; }
    //! Set the longitude of the antenna
    void set_longitude (const Angle& _longitude) { longitude = _longitude; }

    //! Get the elevation of the antenna in metres
    float get_elevation() const { return elevation; }
    //! Set the elevation of the antenna in metres
    void set_elevation (float _elevation) { elevation = _elevation; }

    //! Set the coordinates of the telescope based on known tempo codes
    void set_coordinates (const std::string& code);
    //! Set the coordinates using the telescope name
    void set_coordinates () { set_coordinates(name); }

    //! Get the type of primary reflector mount
    Mount get_mount() const { return mount; }
    //! Set the type of primary reflector mount
    void set_mount (Mount _mount) { mount = _mount; }

    //! Get the type of primary reflector
    Reflector get_primary() const { return primary; }
    //! Set the type of primary reflector
    void set_primary (Reflector _primary) { primary = _primary; }

    //! Get the type of focus
    Focus get_focus() const { return focus; }
    //! Set the type of focus
    void set_focus (Focus _focus) { focus = _focus; }

    //! Get a Directional object set up for this telescope
    Directional* get_Directional() const;

  protected:

    //! Name of the observatory and/or primary reflector
    std::string name;

    //! Latitude of the antenna
    Angle latitude;

    //! Longitude of the antenna
    Angle longitude;

    //! Elevation of the antenna in metres
    float elevation;

    //! Mount type of primary reflector
    Mount mount;

    //! Type of primary reflector
    Reflector primary;

    //! Type of focus
    Focus focus;

  };

}

#endif
