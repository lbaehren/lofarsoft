//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2010 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Util/genutil/Barycentre.h,v $
   $Revision: 1.2 $
   $Date: 2010/09/22 02:15:24 $
   $Author: straten $ */

#ifndef __Baricentre_H
#define __Baricentre_H

#include "MJD.h"
#include "sky_coord.h"
#include "Vector.h"

class Barycentre
{
 public:

  Barycentre ();

  void set_epoch (const MJD&);
  void set_coordinates (const sky_coord&);
  void set_observatory_xyz (double x, double y, double z);

  double get_Doppler () const;
  MJD    get_barycentric_epoch () const;

  bool verbose;

 private:

  MJD epoch;
  sky_coord coordinates;
  Vector<3, double> observatory;

  void build () const;
  mutable bool built;
  mutable double Doppler;
  mutable MJD barycentric_epoch;
};

#endif
