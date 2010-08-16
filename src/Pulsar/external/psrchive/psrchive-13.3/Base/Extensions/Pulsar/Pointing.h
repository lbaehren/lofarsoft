//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Base/Extensions/Pulsar/Pointing.h,v $
   $Revision: 1.11 $
   $Date: 2008/11/27 06:16:15 $
   $Author: straten $ */

#ifndef __Pulsar_Pointing_h
#define __Pulsar_Pointing_h

#include "Pulsar/IntegrationExtension.h"
#include "Angle.h"
#include "Estimate.h"
#include "TextInterface.h"

namespace Pulsar {
  
  //! Telescope pointing parameters recorded during the observation
  class Pointing : public Pulsar::Integration::Extension {
    
  public:
    
    //! Default constructor
    Pointing ();

    //! Copy constructor
    Pointing (const Pointing& extension);

    //! Assignment operator
    const Pointing& operator= (const Pointing& extension);
    
    //! Destructor
    ~Pointing ();

    //! Clone method
    Pointing* clone () const { return new Pointing( *this ); }
    
    //! Return a text interfaces that can be used to access this instance
    TextInterface::Parser* get_interface();
    
    // Text interface to a Pointing instance
    class Interface : public TextInterface::To<Pointing>
    {
      public:
	  Interface( Pointing *s_instance = NULL );
    };

    //! Addition operator
    const Pointing& operator += (const Pointing& extension);

    //! Integrate information from another Integration
    void integrate (const Integration* subint);

    //! Update information based on the provided Integration
    void update (const Integration* subint);

    //! Update information based on the provided Integration and Archive
    void update (const Integration* subint, const Archive *archive);


    //! Set the LST (in seconds) at subint centre
    void set_local_sidereal_time (double seconds);
    //! Get the LST (in seconds) at subint centre
    double get_local_sidereal_time () const;

    //! Set the RA (J2000) at subint centre
    void set_right_ascension (const Angle&);
    //! Get the RA (J2000) at subint centre
    Angle get_right_ascension () const;

    //! Set the DEC (J2000) at subint centre
    void set_declination (const Angle&);
    //! Get the DEC (J2000) at subint centre
    Angle get_declination () const;

    //! Set the Gal longitude at subint centre
    void set_galactic_longitude (const Angle&);
    //! Get the Gal longitude at subint centre
    Angle get_galactic_longitude () const;

    //! Set the Gal latitude at subint centre
    void set_galactic_latitude (const Angle&);
    //! Get the Gal latitude at subint centre
    Angle get_galactic_latitude () const;

    //! Set the Feed angle at subint centre
    void set_feed_angle (const Angle&);
    //! Get the Feed angle at subint centre
    Angle get_feed_angle () const;

    //! Set the Position angle of feed at subint centre
    void set_position_angle (const Angle&);
    //! Get the Position angle of feed at subint centre
    Angle get_position_angle () const;

    //! Set the Parallactic angle at subint centre
    void set_parallactic_angle (const Angle&);
    //! Get the Parallactic angle at subint centre
    Angle get_parallactic_angle () const;

    //! Set the Telescope azimuth at subint centre
    void set_telescope_azimuth (const Angle&);
    //! Get the Telescope azimuth at subint centre
    Angle get_telescope_azimuth () const;

    //! Set the Telescope zenith angle at subint centre
    void set_telescope_zenith (const Angle&);
    //! Get the Telescope zenith angle at subint centre
    Angle get_telescope_zenith () const;


  protected:

    //! LST (in seconds) at subint centre
    MeanRadian<double> local_sidereal_time;
    
    //! RA (J2000, in turns) at subint centre
    MeanRadian<double> right_ascension;

    //! DEC (J2000, in turns) at subint centre
    MeanRadian<double> declination;

    //! Gal longitude at subint centre
    MeanRadian<double> galactic_longitude;

    //! Gal latitude at subint centre
    MeanRadian<double> galactic_latitude;

    //! Feed angle at subint centre
    MeanRadian<double> feed_angle;

    //! Position angle of feed at subint centre
    MeanRadian<double> position_angle;

    //! Parallactic angle at subint centre
    MeanRadian<double> parallactic_angle;

    //! Telescope azimuth at subint centre
    MeanRadian<double> telescope_azimuth;

    //! Telescope zenith angle at subint centre
    MeanRadian<double> telescope_zenith;

  };
  
}

#endif

