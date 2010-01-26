/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "sky_coord.h"

#include <slalib.h>

// redwards -- function to construct from Galactic coordinates
const sky_coord&
sky_coord::setGalactic(AnglePair &gal)
{
  double l, b;
  double ra, dec;

  l = gal.angle1.getRadians();
  b = gal.angle2.getRadians();

  slaGaleq (l, b, &ra, &dec);

  angle1.setRadians(ra);
  angle2.setRadians(dec);
  return *this;
}

AnglePair
sky_coord::getGalactic() const
{
  double l, b;
  double ra, dec;
  AnglePair gal;

  ra = angle1.getRadians();
  dec= angle2.getRadians();

  slaEqgal (ra, dec, &l, &b);

  gal.angle1.setRadians(l);
  gal.angle2.setRadians(b);
  return gal;
}

