//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/Polarimetry/Pulsar/Parallactic.h,v $
   $Revision: 1.6 $
   $Date: 2008/07/04 12:18:56 $
   $Author: straten $ */

#ifndef __Calibration_Parallactic_H
#define __Calibration_Parallactic_H

#include "MEAL/Rotation1.h"
#include "Directional.h"

namespace Calibration {

  //! Parallactic angle transformation of receiver feed
  /*! The parallactic angle is also known as the Vertical Angle (VA).
    According to http://www.ucolick.org/~sla/deimos/swpdr/va.html

    "Note that in many applications this angle is mis-named the
    'Parallactic Angle'.  Lately this has been promoted by
    terminology found in the extremely useful SLALIB software by Pat
    Wallace, but the misnomer can be traced back at least as far as
    Spherical Astronomy by Smart.  The correct usage of Parallactic
    Angle is in the context of the Equatorial and Ecliptic coordinate
    systems.  In that context it describes the orientation of the
    ellipse that stellar coordinates traverse due to annual
    parallax.  The term 'Parallactic Angle' should not be applied in
    the context of the Horizon system."

    Nevertheless, the term 'Parallactic' is used to name this class.
    The following description also comes from the same web-site:

    "The declination of an object is delta, with north being positive.
    The latitude of an observer is phi, with north being positive.
    The Hour Angle of an object is HA; zero on the meridian, negative
    to the east, and positive to the west.  In the equatorial (RA,Dec)
    coordinate system the position angle of a directed vector on the
    sky is measured from equatorial North toward equatorial East; this
    is EquPA.  In the Horizon (Alt-Az) coordinate system the position
    angle of a directed vector on the sky is measured from Up toward
    an upright observer's Left; this is HorPA. [...]  The Vertical
    Angle (VA) is defined as the PA of the local vertical as expressed
    in the equatorial system: VA = EquPA - HorPA"

    This class represents the transformation from EquPA to HorPA as a
    rotation about the Stokes V axis by -VA.
  */

  class Parallactic : public MEAL::Rotation1
  {

  public:

    //! Default constructor
    Parallactic ();

    //! Set the directional antenna
    void set_directional (Directional*);

    //! Get the directional antenna
    Directional* get_directional ();

    //! Get the directional antenna
    const Directional* get_directional () const;

    //! Set the MJD
    void set_epoch (const MJD& epoch);

    //! Get the MJD
    MJD get_epoch () const;

    //! Set the hour angle in radians
    void set_hour_angle (double radians);

    //! Get the hour angle in radians
    double get_hour_angle () const;

  protected:

    Reference::To<Directional> directional;

  };

}

#endif
