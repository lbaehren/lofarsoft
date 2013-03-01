//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Util/genutil/Meridian.h,v $
   $Revision: 1.2 $
   $Date: 2008/07/07 00:27:02 $
   $Author: straten $ */

#ifndef __Meridian_H
#define __Meridian_H

#include "Directional.h"

//! Meridian mounted antenna, with first rotation in meridianal plane
/*! Also known as x-y mount */

class Meridian : public Directional
{  
public:
  
  //! Get the x angle in radians
  double get_x () const;

  //! Get the y angle in radians
  double get_y () const;

  //! Get the name of the mount
  std::string get_name () const;

protected:

  //! Get the receptor basis in the reference frame of the observatory
  Matrix<3,3,double> get_basis (const Vector<3,double>& from_source) const;

  //! The telescope x in radians
  mutable double x;

  //! The telescope y in radians
  mutable double y;

};

#endif
