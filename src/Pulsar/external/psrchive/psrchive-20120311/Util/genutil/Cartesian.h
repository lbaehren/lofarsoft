//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2000 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Util/genutil/Cartesian.h,v $
   $Revision: 1.12 $
   $Date: 2006/10/06 21:13:54 $
   $Author: straten $ */

#ifndef __CARTESIAN_H
#define __CARTESIAN_H

#include <iostream>
#include "Angle.h"

class Cartesian
{
 public:
  double x, y, z;

  Cartesian (double ux = 0.0, double uy = 0.0, double uz = 0.0)
    { x=ux; y=uy; z=uz; }

  Cartesian (const Cartesian& u)
    { x=u.x; y=u.y; z=u.z; }

  Cartesian (const AnglePair& spherical);

  Cartesian & operator =  (const Cartesian& u)
    { x=u.x; y=u.y; z=u.z; return *this; }

  Cartesian & operator += (const Cartesian& u)
    { x+=u.x; y+=u.y; z+=u.z; return *this; }

  Cartesian & operator -= (const Cartesian& u)
    { x-=u.x; y-=u.y; z-=u.z; return *this; }

  Cartesian & operator *= (double a)
    { x*=a; y*=a; z*=a; return *this; }

  Cartesian & operator /= (double a)
    { return operator *= (1.0/a); }

  double& operator[] (int d)
    { return *(&x+d); }

  operator double* ()
    { return &x; }

  const double& operator [] (int d) const
    { return *(&x+d); }

  const friend Cartesian operator + (Cartesian a, const Cartesian& b)
    { return a+=b; }

  const friend Cartesian operator - (Cartesian a, const Cartesian& b)
    { return a-=b; }

  const friend Cartesian operator - (const Cartesian& b)
    { return Cartesian (-b.x, -b.y, -b.z); }

  const friend Cartesian operator * (Cartesian a, double c)
    { return a*=c; }

  const friend Cartesian operator / (Cartesian a, double c)
    { return a*=1.0/c; }

  const friend Cartesian operator * (double a, const Cartesian& v)
    { return v * a; }

  // just does a piece-wise division (x/x, y/y, z/z)
  const friend Cartesian pdiv (const Cartesian &, const Cartesian &);
  // just does a piece-wise multiplication (x*x, y*y, z*z)
  const friend Cartesian pmult (const Cartesian &, const Cartesian &);

  // cross product
  const friend Cartesian cross (const Cartesian& c1, const Cartesian& c2)
    { return Cartesian ( c1.y * c2.z - c1.z * c2.y,
			 c1.z * c2.x - c1.x * c2.z,
			 c1.x * c2.y - c1.y * c2.x ); }

  // cross product
  const friend Cartesian operator % (const Cartesian &, const Cartesian &);
  // scalar product
  friend double operator * (const Cartesian &, const Cartesian &);

  friend int operator == (const Cartesian &, const Cartesian &);
  friend int operator != (const Cartesian &, const Cartesian &);

  friend Cartesian min (const Cartesian& cart1, const Cartesian& cart2);
  friend Cartesian max (const Cartesian& cart1, const Cartesian& cart2);
  friend void diagonalize (Cartesian& bottom_left, Cartesian& upper_right);

  double modSquared () const
    { return x*x + y*y + z*z; }

  double mod () const
    { return sqrt (x*x + y*y + z*z); }

  // rotates the point about the specifies axis
  void x_rot (const Angle& phi);
  void y_rot (const Angle& phi);
  void z_rot (const Angle& phi);

  // rotates the point about an arbitrary vector
  void rot (const Cartesian& r_vect, const Angle& phi);

  static Angle angularSeparation (const Cartesian& c1, const Cartesian& c2);

  static void rotate (double& x1, double& x2, double radians);

};

std::ostream& operator<< (std::ostream& ostr, const Cartesian&);

#endif // __CARTESIAN_H
