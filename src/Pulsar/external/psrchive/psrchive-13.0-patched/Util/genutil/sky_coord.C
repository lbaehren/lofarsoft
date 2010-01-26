/***************************************************************************
 *
 *   Copyright (C) 1999 by Russell Edwards
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "sky_coord.h"

#include <stdio.h>
#include <math.h>

void sky_coord::init ()
{
  angle1.setWrapPoint (2*M_PI);
}

sky_coord::sky_coord ()
{
  init ();
}

sky_coord::sky_coord (const std::string& astr) : AnglePair(astr)
{
  init ();
}

sky_coord::sky_coord (const sky_coord & co) 
{ 
  init ();
  *this = co; 
}

// as long as sky_coord is empty, why not simply accept AnglePair?
sky_coord::sky_coord (const AnglePair & co)
{
  init ();
  AnglePair::operator=(co);
}


sky_coord::sky_coord (const Angle & a1, const Angle & a2) : AnglePair(a1,a2)
{
  init ();
}

sky_coord::sky_coord (const double d1, const double d2) : AnglePair(d1,d2)
{
  init ();
}

// ra and dec given in milliseconds of an hour
// JRa and JDec are stored in radians
const sky_coord& sky_coord::setJRaDecMS(long int ra, long int dec)
{
  angle1 = double(ra)  * M_PI / MilliSecin12Hours;
  angle2 = double(dec) * M_PI / MilliSecin12Hours;
  return *this;
}

//! construct from right ascension and declination
sky_coord hmsdms (const std::string& ra, const std::string& dec)
{
  sky_coord r;
  r.setHMSDMS (ra.c_str(), dec.c_str());
  return r;
}

std::ostream& operator << (std::ostream& ostr, const sky_coord& coord)
{
  return ostr << coord.getHMSDMS (3,2,false);
}

std::istream& operator >> (std::istream& istr, sky_coord& coord)
{
  std::string parse;
  istr >> parse;
  if (coord.setHMSDMS (parse.c_str()) < 0)
    istr.setstate (std::ios::failbit);
  return istr;
}
