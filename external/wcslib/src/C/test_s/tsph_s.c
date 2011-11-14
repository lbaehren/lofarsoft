/*============================================================================
*
*   WCSLIB 4.2 - an implementation of the FITS WCS convention.
*   Copyright (C) 1995-2005, Mark Calabretta
*
*   WCSLIB is free software; you can redistribute it and/or modify it under
*   the terms of the GNU General Public License as published by the Free
*   Software Foundation; either version 2 of the License, or (at your option)
*   any later version.
*
*   WCSLIB is distributed in the hope that it will be useful, but WITHOUT ANY
*   WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
*   FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
*   details.
*
*   You should have received a copy of the GNU General Public License along
*   with WCSLIB; if not, write to the Free Software Foundation, Inc.,
*   59 Temple Place, Suite 330, Boston, MA  02111-1307, USA
*
*   Correspondence concerning WCSLIB may be directed to:
*      Internet email: mcalabre@atnf.csiro.au
*      Postal address: Dr. Mark Calabretta
*                      Australia Telescope National Facility, CSIRO
*                      PO Box 76
*                      Epping NSW 1710
*                      AUSTRALIA
*
*   Author: Mark Calabretta, Australia Telescope National Facility
*   http://www.atnf.csiro.au/~mcalabre/index.html
*   $Id: tsph.c,v 2.10 2002/04/03 01:25:29 mcalabre Exp $
*=============================================================================
*
*   tsph tests the forward and reverse spherical coordinate transformation
*   routines for closure.
*
*---------------------------------------------------------------------------*/

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "wcstrig.h"
#include "sph.h"

#ifndef __STDC__
#ifndef const
#define const
#endif
#endif


int main()

{
   int   err, j, lat, lng;
   double coslat, lng1, lng2, eul[5], lat1, lat2, phi, theta, zeta;
   const double tol = 1.0e-12;


   printf("\nTesting closure of WCSLIB coordinate transformation routines\n");
   printf("------------------------------------------------------------\n");


   /* Set reference angles. */
   eul[0] =  90.0;
   eul[1] =  30.0;
   eul[2] = -90.0;
   printf("\n%s\n%s%10.4f%10.4f%10.4f\n",
      "Celestial longitude and latitude of the native pole, and native",
      "longitude of the celestial pole (degrees):", eul[0], eul[1], eul[2]);

   eul[3] = cosd(eul[1]);
   eul[4] = sind(eul[1]);

   printf ("Reporting tolerance:%6.1g degrees of arc.\n", tol);

   for (lat = 90; lat >= -90; lat--) {
      lat1 = (double)lat;
      coslat = cosd(lat1);

      for (lng = -180; lng <= 180; lng++) {
         lng1 = (double)lng;

         if (err = sphfwd(lng1, lat1, eul, &phi, &theta)) {
            printf ("lng1 =%20.15f  lat1 =%20.15f  Error%3d\n",
               lng1, lat1, err);
            continue;
         }

         if (err = sphrev(phi, theta, eul, &lng2, &lat2)) {
            printf("lng1 =%20.15f  lat1 =%20.15f\n", lng1, lat1);
            printf(" phi =%20.15f theta =%20.15f  Error%3d\n",
                   phi, theta, err);
            continue;
         }

         if (fabs(lat2-lat1) > tol || (fabs(lng2-lng1)-360.0)*coslat > tol) {
            printf("Unclosed: lng1 =%20.15f  lat1 =%20.15f\n", lng1, lat1);
            printf("           phi =%20.15f theta =%20.15f\n", phi, theta);
            printf("          lng2 =%20.15f  lat2 =%20.15f\n", lng2, lat2);
         }
      }
   }


   /* Test closure at points close to the pole. */
   for (j = -1; j <= 1; j += 2) {
      zeta = 1.0;
      lng1 = -180.0;

      for (lat = 0; lat < 12; lat++) {
         lat1 = (double)j*(90.0 - zeta);

         if (err = sphfwd(lng1, lat1, eul, &phi, &theta)) {
            printf ("lng1 =%20.15f  lat1 =%20.15f  Error%3d\n",
               lng1, lat1, err);
            continue;
         }

         if (err = sphrev(phi, theta, eul, &lng2, &lat2)) {
            printf("lng1 =%20.15f  lat1 =%20.15f\n", lng1, lat1);
            printf(" phi =%20.15f theta =%20.15f  Error%3d\n",
                   phi, theta, err);
            continue;
         }

         if (fabs(lat2-lat1) > tol || (fabs(lng2-lng1)-360.0)*coslat > tol) {
            printf("Unclosed: lng1 =%20.15f  lat1 =%20.15f\n", lng1, lat1);
            printf("           phi =%20.15f theta =%20.15f\n", phi, theta);
            printf("          lng2 =%20.15f  lat2 =%20.15f\n", lng2, lat2);
         }

         zeta /= 10.0;
         lng1 += 30.0;
      }
   }

   return 0;
}
