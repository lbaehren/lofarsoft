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
*   $Id: tproj2.c,v 2.12 2002/04/03 01:25:29 mcalabre Exp $
*=============================================================================
*
*   tproj2 tests projection routines by plotting test grids using PGPLOT.
*
*---------------------------------------------------------------------------*/

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <cpgplot.h>
#include "prj.h"


int main()

{
   void prjplt();
   int    j;
   char text[80], text1[80], text2[80];
   struct prjprm prj;


   printf("\nTesting WCSLIB spherical projection routines\n");
   printf("--------------------------------------------\n");

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

   printf("\n");


   /* PGPLOT initialization. */
   strcpy(text, "/xwindow");
   cpgbeg(0, text, 1, 1);

   /* Define pen colours. */
   cpgscr(0, 0.00f, 0.00f, 0.00f);
   cpgscr(1, 1.00f, 1.00f, 0.00f);
   cpgscr(2, 1.00f, 1.00f, 1.00f);
   cpgscr(3, 0.50f, 0.50f, 0.80f);
   cpgscr(4, 0.80f, 0.50f, 0.50f);
   cpgscr(5, 0.80f, 0.80f, 0.80f);
   cpgscr(6, 0.50f, 0.50f, 0.80f);
   cpgscr(7, 0.80f, 0.50f, 0.50f);
   cpgscr(8, 0.30f, 0.50f, 0.30f);

   strcpy(text1, "\n%s projection\n");
   strcpy(text2, "\n%s projection\nParameters:");

   prjini(&prj);

   /* AZP: zenithal/azimuthal perspective. */
   prj.p[1] =   2.0;
   prj.p[2] =  30.0;
   printf(text2, "Zenithal/azimuthal perspective");
   printf("%12.5f%12.5f\n", prj.p[1], prj.p[2]);
   prjplt("AZP", 90, -90, &prj);

   /* SZP: slant zenithal perspective. */
   prj.p[1] =   2.0;
   prj.p[2] = 210.0;
   prj.p[3] =  60.0;
   printf(text2, "Slant zenithal perspective");
   printf("%12.5f%12.5f%12.5f\n", prj.p[1], prj.p[2], prj.p[3]);
   prjplt("SZP", 90, -90, &prj);

   /* TAN: gnomonic. */
   printf(text1, "Gnomonic");
   prjplt("TAN", 90,   5, &prj);

   /* STG: stereographic. */
   printf(text1, "Stereographic");
   prjplt("STG", 90, -85, &prj);

   /* SIN: orthographic. */
   prj.p[1] = -0.3;
   prj.p[2] =  0.5;
   printf(text2, "Orthographic/synthesis");
   printf("%12.5f%12.5f\n", prj.p[1], prj.p[2]);
   prjplt("SIN", 90, -90, &prj);

   /* ARC: zenithal/azimuthal equidistant. */
   printf(text1, "Zenithal/azimuthal equidistant");
   prjplt("ARC", 90, -90, &prj);

   /* ZPN: zenithal/azimuthal polynomial. */
   prj.p[0] =  0.05000;
   prj.p[1] =  0.95000;
   prj.p[2] = -0.02500;
   prj.p[3] = -0.15833;
   prj.p[4] =  0.00208;
   prj.p[5] =  0.00792;
   prj.p[6] = -0.00007;
   prj.p[7] = -0.00019;
   prj.p[8] =  0.00000;
   prj.p[9] =  0.00000;
   printf(text2, "Zenithal/azimuthal polynomial");
   printf("%12.5f%12.5f%12.5f%12.5f%12.5f\n",
      prj.p[0], prj.p[1], prj.p[2], prj.p[3], prj.p[4]);
   printf("           %12.5f%12.5f%12.5f%12.5f%12.5f\n",
      prj.p[5], prj.p[6], prj.p[7], prj.p[8], prj.p[9]);
   prjplt("ZPN", 90,  10, &prj);

   /* ZEA: zenithal/azimuthal equal area. */
   printf(text1, "Zenithal/azimuthal equal area");
   prjplt("ZEA", 90, -90, &prj);

   /* AIR: Airy's zenithal projection. */
   prj.p[1] = 45.0;
   printf(text2, "Airy's zenithal");
   printf("%12.5f\n", prj.p[1]);
   prjplt("AIR", 90, -85, &prj);

   /* CYP: cylindrical perspective. */
   prj.p[1] = 3.0;
   prj.p[2] = 0.8;
   printf(text2, "Cylindrical perspective");
   printf("%12.5f%12.5f\n", prj.p[1], prj.p[2]);
   prjplt("CYP", 90, -90, &prj);

   /* CEA: cylindrical equal area. */
   prj.p[1] = 0.75;
   printf(text2, "Cylindrical equal area");
   printf("%12.5f\n", prj.p[1]);
   prjplt("CEA", 90, -90, &prj);

   /* CAR: Cartesian. */
   printf(text1, "Cartesian");
   prjplt("CAR", 90, -90, &prj);

   /* MER: Mercator's. */
   printf(text1, "Mercator's");
   prjplt("MER", 85, -85, &prj);

   /* SFL: Sanson-Flamsteed. */
   printf(text1, "Sanson-Flamsteed (global sinusoid)");
   prjplt("SFL", 90, -90, &prj);

   /* PAR: parabolic. */
   printf(text1, "Parabolic");
   prjplt("PAR", 90, -90, &prj);

   /* MOL: Mollweide's projection. */
   printf(text1, "Mollweide's");
   prjplt("MOL", 90, -90, &prj);

   /* AIT: Hammer-Aitoff. */
   printf(text1, "Hammer-Aitoff");
   prjplt("AIT", 90, -90, &prj);

   /* COP: conic perspective. */
   prj.p[1] =  60.0;
   prj.p[2] =  15.0;
   printf(text2, "Conic perspective");
   printf("%12.5f%12.5f\n", prj.p[1], prj.p[2]);
   prjplt("COP", 90, -25, &prj);

   /* COD: conic equidistant. */
   prj.p[1] = -60.0;
   prj.p[2] =  15.0;
   printf(text2, "Conic equidistant");
   printf("%12.5f%12.5f\n", prj.p[1], prj.p[2]);
   prjplt("COD", 90, -90, &prj);

   /* COE: conic equal area. */
   prj.p[1] =  60.0;
   prj.p[2] = -15.0;
   printf(text2, "Conic equal area");
   printf("%12.5f%12.5f\n", prj.p[1], prj.p[2]);
   prjplt("COE", 90, -90, &prj);

   /* COO: conic orthomorphic. */
   prj.p[1] = -60.0;
   prj.p[2] = -15.0;
   printf(text2, "Conic orthomorphic");
   printf("%12.5f%12.5f\n", prj.p[1], prj.p[2]);
   prjplt("COO", 85, -90, &prj);

   /* BON: Bonne's projection. */
   prj.p[1] = 30.0;
   printf(text2, "Bonne's");
   printf("%12.5f\n", prj.p[1]);
   prjplt("BON", 90, -90, &prj);

   /* PCO: polyconic. */
   printf(text1, "Polyconic");
   prjplt("PCO", 90, -90, &prj);

   /* TSC: tangential spherical cube. */
   printf(text1, "Tangential spherical cube");
   prjplt("TSC", 90, -90, &prj);

   /* CSC: COBE quadrilateralized spherical cube. */
   printf(text1, "COBE quadrilateralized spherical cube");
   prjplt("CSC", 90, -90, &prj);

   /* QSC: quadrilateralized spherical cube. */
   printf(text1, "Quadrilateralized spherical cube");
   prjplt("QSC", 90, -90, &prj);

   cpgask(0);
   cpgend();

   return 0;
}


/*----------------------------------------------------------------------------
*   PRJPLT draws a 15 degree coordinate grid.
*
*   Given:
*      pcode[4]  char     Projection mnemonic.
*      north     int      Northern cutoff latitude, degrees.
*      south     int      Southern cutoff latitude, degrees.
*
*   Given and returned:
*      prj       prjprm*  Projection parameters.
*---------------------------------------------------------------------------*/

void prjplt(pcode, north, south, prj)

char   pcode[4];
int    north, south;
struct prjprm *prj;

{
   char   text[80];
   int    ci, cubic, ilat, ilng;
   register int j;
   float  xr[512], yr[512];
   double lat, lng, x, y;

   prj->flag = 0;
   prj->r0 = 0.0;

   strcpy(prj->code, pcode);
   prjset(prj);

   printf("Plotting %s; latitudes%3d to%4d.\n", prj->code, north, south);

   cpgask(0);

   cubic = prj->flag/100 == 7;
   if (cubic) {
      cpgenv(-335.0f, 65.0f, -200.0f, 200.0f, 1, -2);
      cpgsci(2);
      sprintf(text,"%s - 15 degree graticule", pcode);
      cpgtext(-340.0f, -220.0f, text);

      cpgsci(8);

      xr[0] =  prj->w[0];
      yr[0] =  prj->w[0];
      xr[1] =  prj->w[0];
      yr[1] =  prj->w[0]*3.0;
      xr[2] = -prj->w[0];
      yr[2] =  prj->w[0]*3.0;
      xr[3] = -prj->w[0];
      yr[3] = -prj->w[0]*3.0;
      xr[4] =  prj->w[0];
      yr[4] = -prj->w[0]*3.0;
      xr[5] =  prj->w[0];
      yr[5] =  prj->w[0];
      xr[6] = -prj->w[0]*7.0;
      yr[6] =  prj->w[0];
      xr[7] = -prj->w[0]*7.0;
      yr[7] = -prj->w[0];
      xr[8] =  prj->w[0];
      yr[8] = -prj->w[0];
      cpgline(9, xr, yr);
   } else {
      cpgenv(-200.0f, 200.0f, -200.0f, 200.0f, 1, -2);
      cpgsci(2);
      sprintf(text,"%s - 15 degree graticule", pcode);
      cpgtext(-240.0f, -220.0f, text);
   }


   ci = 1;
   for (ilng = -180; ilng <= 180; ilng+=15) {
      if (++ci > 7) ci = 2;

      lng = (double)ilng;

      cpgsci(ilng?ci:1);

      j = 0;
      for (ilat = north; ilat >= south; ilat--) {
         lat = (double)ilat;

         if (prj->prjfwd(lng, lat, prj, &x, &y)) {
            if (j > 1) cpgline(j, xr, yr);
            j = 0;
            continue;
         }

         if (cubic && j > 0) {
            if (fabs(x+xr[j-1]) > 2.0 || fabs(y-yr[j-1]) > 5.0) {
               if (j > 1) cpgline(j, xr, yr);
               j = 0;
            }
         }

         xr[j] = -x;
         yr[j] =  y;
         j++;
      }

      cpgline(j, xr, yr);
   }

   ci = 1;
   for (ilat = -90; ilat <= 90; ilat += 15) {
      if (++ci > 7) ci = 2;

      if (ilat > north) continue;
      if (ilat < south) continue;

      lat = (double)ilat;

      cpgsci(ilat?ci:1);

      j = 0;
      for (ilng = -180; ilng <= 180; ilng++) {
         lng = (double)ilng;

         if (prj->prjfwd(lng, lat, prj, &x, &y)) {
            if (j > 1) cpgline(j, xr, yr);
            j = 0;
            continue;
         }

         if (cubic && j > 0) {
            if (fabs(x+xr[j-1]) > 2.0 || fabs(y-yr[j-1]) > 5.0) {
               if (j > 1) cpgline(j, xr, yr);
               j = 0;
            }
         }

         xr[j] = -x;
         yr[j] =  y;
         j++;
      }

      cpgline(j, xr, yr);
   }

   cpgsci(1);
   xr[0] = 0.0f;
   yr[0] = 0.0f;
   cpgpt(1, xr, yr, 21);


   prjini(prj);

   cpgask(1);
   cpgpage();

   return;
}
