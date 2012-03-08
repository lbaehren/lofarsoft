//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Util/genutil/Horizon.h,v $
   $Revision: 1.7 $
   $Date: 2008/07/07 00:27:02 $
   $Author: straten $ */

#ifndef __Horizon_H
#define __Horizon_H

#include "Directional.h"

//! Horizon mounted antenna with first rotation in horizontal plane
/*! Also known as altazimuth, az-el, Dobsonian, or even x-y mount */
class Horizon : public Directional
{  
public:
  
  //! Get the azimuth angle in radians
  double get_azimuth () const;

  //! Get the elevation angle in radians
  double get_elevation () const;

  //! Get the zenith angle in radians
  double get_zenith () const;

  //! Get the name of the mount
  std::string get_name () const;

protected:

  //! Get the receptor basis in the reference frame of the observatory
  Matrix<3,3,double> get_basis (const Vector<3,double>& from_source) const;

  //! The telescope azimuth in radians
  mutable double azimuth;

  //! The telescope elevation in radians
  mutable double elevation;

};

#endif
