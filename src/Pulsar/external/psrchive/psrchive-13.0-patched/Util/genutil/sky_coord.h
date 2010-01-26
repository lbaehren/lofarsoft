/***************************************************************************
 *
 *   Copyright (C) 1999 by Russell Edwards
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
/* $Source: /cvsroot/psrchive/psrchive/Util/genutil/sky_coord.h,v $
   $Revision: 1.12 $
   $Date: 2007/07/18 02:51:32 $
   $Author: straten $ */

#ifndef __SKY_COORD_H
#define __SKY_COORD_H


#include "Angle.h"

// Class for sky coordinates. Inherits AnglePair, where members
// angle1 and angle2 are J2000.0 right ascension and declination
// respectively.
class sky_coord  : public AnglePair
{
 public:
   sky_coord ();

   sky_coord (const std::string& astr);
   sky_coord (const sky_coord & co);

   sky_coord (const AnglePair & co);

   sky_coord& operator= (const AnglePair & co)
     { AnglePair::operator=(co); return *this; }

   sky_coord (const Angle & a1, const Angle & a2);
   sky_coord (const double d1, const double d2);

   const sky_coord& setJRaDecMS(long int ra, long int dec);

   const sky_coord& setGalactic(AnglePair &gal);

   AnglePair getGalactic() const;

   AnglePair & getRaDec() { return *this; }

   Angle & ra()  { return angle1; }
   Angle & dec() { return angle2; }

   // const versions
   const AnglePair & getRaDec() const { return *this; }
   const Angle & ra() const  { return angle1; }
   const Angle & dec() const { return angle2; }

 private:

   //! Set the wrap point for all of the constructors
   void init ();

};

//! insertion operator
std::ostream& operator<< (std::ostream& ostr, const sky_coord& coord);

//! extraction operator
std::istream& operator>> (std::istream& istr, sky_coord& coord);

//! construct from right ascension and declination
sky_coord hmsdms (const std::string& ra, const std::string& dec);

#endif //SKY_COORD_H
