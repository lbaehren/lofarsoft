//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Util/genutil/Mount.h,v $
   $Revision: 1.5 $
   $Date: 2009/10/17 07:55:54 $
   $Author: straten $ */

#ifndef __Mount_H
#define __Mount_H

#include "Reference.h"
#include "MJD.h"
#include "sky_coord.h"
#include "Matrix.h"

//! Calculates horizon pointing parameters using SLALIB

class Mount : public Reference::Able
{
  
public:
  
  //! Default constructor
  Mount ();

  //! Destructor
  virtual ~Mount ();

  //! Set the coordinates of the source
  void set_source_coordinates (const sky_coord& coords);
  
  //! Set the latitude of the observatory in radians
  void set_observatory_latitude (double latitude);
  double get_observatory_latitude () const;

  //! Set the longitude of the observatory in radians East of Greenwich
  void set_observatory_longitude (double longitude);
  double get_observatory_longitude () const;

  //! Set the epoch in Modified Julian Days
  void set_epoch (const MJD& epoch);
  MJD get_epoch () const;

  //! Set the hour_angle in radians
  void set_hour_angle (double rad);
  double get_hour_angle () const;

  //! Set the LST in radians
  void set_local_sidereal_time (double rad);
  double get_local_sidereal_time () const;

  //! Get the name of the mount
  virtual std::string get_name () const = 0;

  //! Get the vertical angle (rotation about the line of sight)
  virtual double get_vertical () const = 0;

protected:

  //! The declination of the source in radians
  double declination;

  //! The right ascension of the source in radians
  double right_ascension;

  //! The latitude of the observatory in radians
  double latitude;

  //! The latitude of the observatory in radians East of Greenwich
  double longitude;

  //! The epoch
  MJD epoch;

  //! The LST in radians
  mutable double lst;

  //! The hour angle in radians
  mutable double hour_angle;

  //! Basis pointing to source in celestial reference frame
  mutable Matrix<3,3,double> source_basis;

  //! Basis pointing to observatory in celestial reference frame
  mutable Matrix<3,3,double> observatory_basis;

  //! Lazily evaluate lst and hour_angle
  virtual void build () const;

  //! Return true when outputs reflect inputs
  bool get_built () const;

};

#endif
