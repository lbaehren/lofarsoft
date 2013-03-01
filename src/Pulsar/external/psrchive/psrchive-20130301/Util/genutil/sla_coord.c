/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "coord.h"

#include <math.h>
#include <slalib.h>

/* **********************************************************************

   az_zen_para - given the J2000 ra and dec, local sidereal time, and
                 latitude, returns the azimuth, zenith, and parallactic
		 angles of the telescope.

		 ra and dec are given in radians
		 lst is given in hours
		 latitude is given in degrees

		 para_angle, tel_az and tel_zen are returned in degrees

   Uses: SLALIB SUBROUTINE sla_ALTAZ (HA, DEC, PHI,
                           AZ, AZD, AZDD, EL, ELD, ELDD, PA, PAD, PADD)

   ********************************************************************** */


int az_zen_para (double ra, double dec, float lst, float latitude,
		 float* tel_az, float* tel_zen, float* para_angle)
{
  double altitude, azimuth, PA;
  double ignore;

  double HA = lst * M_PI/12.0 - ra;     /* hour angle */
  double rad2deg = 180.0 / M_PI;
  double dlat = latitude / rad2deg;

  if (lst < 0.0 || lst > 24.0)
    return -1;

  slaAltaz (HA, dec, dlat, 
	    &azimuth,  &ignore, &ignore,
	    &altitude, &ignore, &ignore,
	    &PA,       &ignore, &ignore);

  *para_angle = (float) PA  * rad2deg;
  *tel_zen = 90.0 - (float) altitude * rad2deg;
  *tel_az = (float) azimuth * rad2deg;

  return 0;
}

