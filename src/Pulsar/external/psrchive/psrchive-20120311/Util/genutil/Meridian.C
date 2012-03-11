/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Meridian.h"

//! Get the x angle in radians
double Meridian::get_x () const
{
  build ();
  return x;
}

//! Get the y angle in radians
double Meridian::get_y () const
{
  build ();
  return y;
}

//! Get the name of the mount
std::string Meridian::get_name () const
{
  return "Meridian";
}

//! Get the receptor basis in the reference frame of the observatory
Matrix<3,3,double> Meridian::get_basis (const Vector<3,double>& from) const
{
  /*
    angle in meridianal plane, positive toward North
  */
  x = - atan2 (from[0], from[2]);

  /*
    radius projected into meridianal plane
  */
  double r = sqrt (from[2]*from[2] + from[0]*from[0]);

  /*
    angle out of meridianal plane, positive toward East
  */
  y = - atan (from[1] / r);

  /*
    receptor basis in the frame of the observatory

    start with dish pointing toward zenith,
    x receptor toward North, y receptor toward East,

    positive rotations about East dip dish toward South, so rotate -x
    positive rotations about North (prime) dip dish toward East, so rotate y
  */
  return transpose( rotation (Vector<3,double>::basis(1), -x) *
		    rotation (Vector<3,double>::basis(0), y) );
}
