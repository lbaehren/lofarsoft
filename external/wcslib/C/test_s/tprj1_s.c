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
*   $Id: tproj1.c,v 2.13 2002/04/03 01:25:29 mcalabre Exp $
*=============================================================================
*
*   tproj1 tests forward and reverse spherical projections for closure.
*
*---------------------------------------------------------------------------*/

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "wcstrig.h"
#include "prj.h"

#ifndef __STDC__
#ifndef const
#define const
#endif
#endif


int main()

{
   void projex();
   register int j;
   const double tol = 1.0e-9;
   struct prjprm prj;

   /* Uncomment the following two lines to raise SIGFPE on floating point
    * exceptions for the Sun FORTRAN compiler.  This signal can be caught
    * within 'dbx' by issuing the command "catch FPE".
    */
/* #include <floatingpoint.h> */
/* ieee_handler("set", "common", SIGFPE_ABORT); */


   printf("\nTesting closure of WCSLIB spherical projection routines\n");
   printf("-------------------------------------------------------\n");

   /* List error messages. */
   printf("\nList of prjset error codes:\n");
   for (j = 1; j <= 2; j++) {
      printf("   %d: %s.\n", j, prjset_errmsg[j]);
   }

   printf("\nList of prjfwd error codes:\n");
   for (j = 1; j <= 4; j++) {
      printf("   %d: %s.\n", j, prjfwd_errmsg[j]);
   }

   printf("\nList of prjrev error codes:\n");
   for (j = 1; j <= 4; j++) {
      printf("   %d: %s.\n", j, prjrev_errmsg[j]);
   }

   printf("\n\n");


   prjini(&prj);

   /* AZP: zenithal/azimuthal perspective. */
   prj.p[1] =   0.5;
   prj.p[2] =  30.0;
   projex("AZP", &prj, 90, 5, tol);

   /* SZP: slant zenithal perspective. */
   prj.p[1] =   0.5;
   prj.p[2] = 210.0;
   prj.p[3] =  60.0;
   projex("SZP", &prj, 90, -90, tol);

   /* TAN: gnomonic. */
   projex("TAN", &prj, 90, 5, tol);

   /* STG: stereographic. */
   projex("STG", &prj, 90, -85, tol);

   /* SIN: orthographic/synthesis. */
   prj.p[1] = -0.3;
   prj.p[2] =  0.5;
   projex("SIN", &prj, 90, 45, tol);

   /* ARC: zenithal/azimuthal equidistant. */
   projex("ARC", &prj, 90, -90, tol);

   /* ZPN: zenithal/azimuthal polynomial. */
   prj.p[0] =  0.00000;
   prj.p[1] =  0.95000;
   prj.p[2] = -0.02500;
   prj.p[3] = -0.15833;
   prj.p[4] =  0.00208;
   prj.p[5] =  0.00792;
   prj.p[6] = -0.00007;
   prj.p[7] = -0.00019;
   prj.p[8] =  0.00000;
   prj.p[9] =  0.00000;
   projex("ZPN", &prj, 90, 10, tol);

   /* ZEA: zenithal/azimuthal equal area. */
   projex("ZEA", &prj, 90, -85, tol);

   /* AIR: Airy's zenithal projection. */
   prj.p[1] = 45.0;
   projex("AIR", &prj, 90, -85, tol);

   /* CYP: cylindrical perspective. */
   prj.p[1] = 3.0;
   prj.p[2] = 0.8;
   projex("CYP", &prj, 90, -90, tol);

   /* CEA: cylindrical equal area. */
   prj.p[1] = 0.75;
   projex("CEA", &prj, 90, -90, tol);

   /* CAR: Cartesian. */
   projex("CAR", &prj, 90, -90, tol);

   /* MER: Mercator's. */
   projex("MER", &prj, 85, -85, tol);

   /* SFL: Sanson-Flamsteed. */
   projex("SFL", &prj, 90, -90, tol);

   /* PAR: parabolic. */
   projex("PAR", &prj, 90, -90, tol);

   /* MOL: Mollweide's projection. */
   projex("MOL", &prj, 90, -90, tol);

   /* AIT: Hammer-Aitoff. */
   projex("AIT", &prj, 90, -90, tol);

   /* COP: conic perspective. */
   prj.p[1] =  60.0;
   prj.p[2] =  15.0;
   projex("COP", &prj, 90, -25, tol);

   /* COE: conic equal area. */
   prj.p[1] =  60.0;
   prj.p[2] = -15.0;
   projex("COE", &prj, 90, -90, tol);

   /* COD: conic equidistant. */
   prj.p[1] = -60.0;
   prj.p[2] =  15.0;
   projex("COD", &prj, 90, -90, tol);

   /* COO: conic orthomorphic. */
   prj.p[1] = -60.0;
   prj.p[2] = -15.0;
   projex("COO", &prj, 85, -90, tol);

   /* BON: Bonne's projection. */
   prj.p[1] = 30.0;
   projex("BON", &prj, 90, -90, tol);

   /* PCO: polyconic. */
   projex("PCO", &prj, 90, -90, tol);

   /* TSC: tangential spherical cube. */
   projex("TSC", &prj, 90, -90, tol);

   /* CSC: COBE quadrilateralized spherical cube. */
   projex("CSC", &prj, 90, -90, 4.0e-2);

   /* QSC: quadrilateralized spherical cube. */
   projex("QSC", &prj, 90, -90, tol);

   return 0;
}


/*----------------------------------------------------------------------------
*   PROJEX exercises the spherical projection routines.
*
*   Given:
*      pcode[4]  char     Projection code.
*      north     int      Northern cutoff latitude, degrees.
*      south     int      Southern cutoff latitude, degrees.
*      tol       double   Reporting tolerance, degrees.
*
*   Given and returned:
*      prj       prjprm*  Projection parameters.
*---------------------------------------------------------------------------*/

void projex(pcode, prj, north, south, tol)

char   pcode[4];
int    north, south;
double tol;
struct prjprm *prj;

{
   int    err, lat, lng;
   register int j;
   double dlat, dlatmx, dlng, dlngmx, dr, drmax, lat1, lat2, lng1, lng2;
   double r, theta, x, x1, x2, y, y1, y2;

   prj->flag = 0;
   prj->r0 = 0.0;

   strcpy(prj->code, pcode);
   prjset(prj);

   printf("Testing %s; latitudes %3d to %4d, reporting tolerance %5.1g deg.\n",
      prj->code, north, south, tol);

   dlngmx = 0.0;
   dlatmx = 0.0;

   /* Uncomment the next line to test alternative initializations of */
   /* projection parameters.                                         */
   /* prj->r0 = R2D; */

   for (lat = north; lat >= south; lat--) {
      lat1 = (double)lat;

      for (lng = -180; lng <= 180; lng++) {
         lng1 = (double)lng;

         if (err = prj->prjfwd(lng1, lat1, prj, &x, &y)) {
            if (err != 4) {
               printf("        %3s: lng1 =%20.15f  lat1 =%20.15f  error %3d\n",
                  pcode, lng1, lat1, err);
            }
            continue;
         }

         if (err = prj->prjrev(x, y, prj, &lng2, &lat2)) {
            printf("        %3s: lng1 =%20.15f  lat1 =%20.15f\n",
               pcode, lng1, lat1);
            printf("                x =%20.15f     y =%20.15f  error%3d\n",
               x, y, err);
            continue;
         }

         dlng = fabs(lng2-lng1);
         if (dlng > 180.0) dlng = fabs(dlng-360.0);
         if (abs(lat) != 90 && dlng > dlngmx) dlngmx = dlng;
         dlat = fabs(lat2-lat1);
         if (dlat > dlatmx) dlatmx = dlat;

         if (dlat > tol) {
            printf("        %3s: lng1 =%20.15f  lat1 =%20.15f\n",
               pcode, lng1, lat1);
            printf("                x =%20.15f     y =%20.15f\n", x, y);
            printf("             lng2 =%20.15f  lat2 =%20.15f\n",
               lng2, lat2);
         } else if (abs(lat) != 90) {
            if (dlng > tol) {
               printf("        %3s: lng1 =%20.15f  lat1 =%20.15f\n",
                  pcode, lng1, lat1);
               printf("                x =%20.15f     y =%20.15f\n", x, y);
               printf("             lng2 =%20.15f  lat2 =%20.15f\n",
                  lng2, lat2);
            }
         }
      }
   }

   printf("             Maximum residual (sky): lng%10.3e   lat%10.3e\n",
      dlngmx, dlatmx);


   /* Test closure at a point close to the reference point. */
   r = 1.0;
   theta = -180.0;

   drmax = 0.0;

   for (j = 1; j <= 12; j++) {
      x1 = r*cosd(theta);
      y1 = r*sind(theta);

      if (err = prj->prjrev(x1, y1, prj, &lng1, &lat1)) {
         printf("        %3s:   x1 =%20.15f    y1 =%20.15f  error%3d\n",
            pcode, x1, y1, err);

      } else if (err = prj->prjfwd(lng1, lat1, prj, &x2, &y2)) {
         printf("        %3s:   x1 =%20.15f    y1 =%20.15f\n",
            pcode, x1, y1);
         printf("              lng =%20.15f   lat =%20.15f  error%3d\n",
            lng1, lat1, err);

      } else {
         dr = sqrt((x2-x1)*(x2-x1) + (y2-y1)*(y2-y1));
         if (dr > drmax) drmax = dr;
         if (dr > tol) {
            printf("        %3s:   x1 =%20.15f    y1 =%20.15f\n",
               pcode, x1, y1);
            printf("              lng =%20.15f   lat =%20.15f\n",
               lng1, lat1);
            printf("               x2 =%20.15f    y2 =%20.15f\n",
               x2, y2);
         }
      }

      r /= 10.0;
      theta += 15.0;
   }

   printf("             Maximum residual (ref):  dR%10.3e\n", drmax);

   prjini(prj);

   return;
}
