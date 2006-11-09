/***************************************************************************
                          threevector.cc  -  description
                             -------------------
    begin                : Tue May 27 2003
    copyright            : (C) 2003 by Tim Huege
    email                : tim.huege@ik.fzk.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <Math/ThreeVector.h>

// ------------------------------------------------------------------ ThreeVector

ThreeVector::~ThreeVector()
{}

// ----------------------------------------------------------------------- Rotate

ThreeVector& ThreeVector::Rotate (const double& parAngle,
				  const Axis parAxis)
{
  ThreeVector srcVec(*this);        // needed for upcoming calculations
  double cosAngle = cos(parAngle);  // for performance
  double sinAngle = sin(parAngle);
  switch (parAxis)
    {
    case  XAxis:
      {
	y=cosAngle*srcVec.GetY()-sinAngle*srcVec.GetZ();
	z=sinAngle*srcVec.GetY()+cosAngle*srcVec.GetZ();
      }
      break;
    case  YAxis:
      {
	x=cosAngle*srcVec.GetX()+sinAngle*srcVec.GetZ();
	z=-sinAngle*srcVec.GetX()+cosAngle*srcVec.GetZ();
      }
      break;
    case  ZAxis:
      {
	x=cosAngle*srcVec.GetX()-sinAngle*srcVec.GetY();
	y=sinAngle*srcVec.GetX()+cosAngle*srcVec.GetY();
      }
      break;
    default: throw Exception();
      break;
    }
  
  return *this;
}
