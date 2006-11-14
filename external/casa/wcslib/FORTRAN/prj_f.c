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
*   $Id: prj_f.c,v 4.2 2005/09/21 13:25:32 cal103 Exp $
*===========================================================================*/

#include <string.h>
#include <prj.h>

#define PRJ_FLAG      100
#define PRJ_CODE      101
#define PRJ_R0        102
#define PRJ_PV        103
#define PRJ_PHI0      104
#define PRJ_THETA0    105
#define PRJ_BOUNDS    106

#define PRJ_NAME      200
#define PRJ_CATEGORY  201
#define PRJ_PVRANGE   202
#define PRJ_SIMPLEZEN 203
#define PRJ_EQUIAREAL 204
#define PRJ_CONFORMAL 205
#define PRJ_GLOBAL    206
#define PRJ_DIVERGENT 207
#define PRJ_X0        208
#define PRJ_Y0        209
#define PRJ_W         210
#define PRJ_N         211

/*--------------------------------------------------------------------------*/

int prjini_(int *prj)

{
   return prjini((struct prjprm *)prj);
}

/*--------------------------------------------------------------------------*/

int prjput_(int *prj, const int *what, const void *value, const int *m)

{
   const char *cvalp;
   const int  *ivalp;
   const double *dvalp;
   struct prjprm *prjp;

   /* Cast pointers. */
   prjp  = (struct prjprm *)prj;
   cvalp = (const char *)value;
   ivalp = (const int *)value;
   dvalp = (const double *)value;

   prjp->flag = 0;

   switch (*what) {
   case PRJ_FLAG:
      prjp->flag = *ivalp;
      break;
   case PRJ_CODE:
      strncpy(prjp->code, cvalp, 3);
      prjp->code[3] = '\0';
      break;
   case PRJ_R0:
      prjp->r0 = *dvalp;
      break;
   case PRJ_PV:
      prjp->pv[*m] = *dvalp;
      break;
   case PRJ_PHI0:
      prjp->phi0 = *dvalp;
      break;
   case PRJ_THETA0:
      prjp->theta0 = *dvalp;
      break;
   case PRJ_BOUNDS:
      prjp->bounds = *ivalp;
      break;
   default:
      return 1;
   }

   return 0;
}

/*--------------------------------------------------------------------------*/

int prjget_(const int *prj, const int *what, void *value)

{
   int m;
   char *cvalp;
   int  *ivalp;
   double *dvalp;
   const struct prjprm *prjp;

   /* Cast pointers. */
   prjp  = (const struct prjprm *)prj;
   cvalp = (char *)value;
   ivalp = (int *)value;
   dvalp = (double *)value;

   switch (*what) {
   case PRJ_FLAG:
      *ivalp = prjp->flag;
      break;
   case PRJ_CODE:
      strncpy(cvalp, prjp->code, 3);
      break;
   case PRJ_R0:
      *dvalp = prjp->r0;
      break;
   case PRJ_PV:
      for (m = 0; m < PVN; m++) {
         *(dvalp++) = prjp->pv[m];
      }
      break;
   case PRJ_PHI0:
      *dvalp = prjp->phi0;
      break;
   case PRJ_THETA0:
      *dvalp = prjp->theta0;
      break;
   case PRJ_BOUNDS:
      *ivalp = prjp->bounds;
      break;
   case PRJ_NAME:
      strncpy(cvalp, prjp->name, 40);
      break;
   case PRJ_CATEGORY:
      *ivalp = prjp->category;
      break;
   case PRJ_PVRANGE:
      *ivalp = prjp->pvrange;
      break;
   case PRJ_SIMPLEZEN:
      *ivalp = prjp->simplezen;
      break;
   case PRJ_EQUIAREAL:
      *ivalp = prjp->equiareal;
      break;
   case PRJ_CONFORMAL:
      *ivalp = prjp->conformal;
      break;
   case PRJ_GLOBAL:
      *ivalp = prjp->global;
      break;
   case PRJ_DIVERGENT:
      *ivalp = prjp->divergent;
      break;
   case PRJ_X0:
      *dvalp = prjp->x0;
      break;
   case PRJ_Y0:
      *dvalp = prjp->y0;
      break;
   case PRJ_W:
      for (m = 0; m < 10; m++) {
         *(dvalp++) = prjp->w[m];
      }
      break;
   case PRJ_N:
      *ivalp = prjp->n;
      break;
   default:
      return 1;
   }

   return 0;
}

/*--------------------------------------------------------------------------*/

int prjprt_(int *prj)

{
   return prjprt((struct prjprm *)prj);
}

/*--------------------------------------------------------------------------*/

#define PRJSET_FWRAP(PRJ) \
   int PRJ##set_(int *prj) \
   {return prjset((struct prjprm *)prj);}


#define PRJS2X_FWRAP(PRJ) \
   int PRJ##s2x_( \
      int *prj, \
      const int *nphi, \
      const int *ntheta, \
      const int *spt, \
      const int *sxy, \
      const double phi[], \
      const double theta[], \
      double x[], \
      double y[], \
      int stat[]) \
   {return PRJ##s2x((struct prjprm *)prj, *nphi, *ntheta, *spt, *sxy, \
                     phi, theta, x, y, stat);}

#define PRJX2S_FWRAP(PRJ) \
   int PRJ##x2s_( \
      int *prj, \
      const int *nx, \
      const int *ny, \
      const int *sxy, \
      const int *spt, \
      const double x[], \
      const double y[], \
      double phi[], \
      double theta[], \
      int stat[]) \
   {return PRJ##x2s((struct prjprm *)prj, *nx, *ny, *sxy, *spt, x, y, \
                     phi, theta, stat);}

#define PRJ_FWRAP(PRJ) \
   PRJSET_FWRAP(PRJ)   \
   PRJS2X_FWRAP(PRJ)   \
   PRJX2S_FWRAP(PRJ)

PRJ_FWRAP(prj)
PRJ_FWRAP(azp)
PRJ_FWRAP(szp)
PRJ_FWRAP(tan)
PRJ_FWRAP(stg)
PRJ_FWRAP(sin)
PRJ_FWRAP(arc)
PRJ_FWRAP(zpn)
PRJ_FWRAP(zea)
PRJ_FWRAP(air)
PRJ_FWRAP(cyp)
PRJ_FWRAP(cea)
PRJ_FWRAP(car)
PRJ_FWRAP(mer)
PRJ_FWRAP(sfl)
PRJ_FWRAP(par)
PRJ_FWRAP(mol)
PRJ_FWRAP(ait)
PRJ_FWRAP(cop)
PRJ_FWRAP(coe)
PRJ_FWRAP(cod)
PRJ_FWRAP(coo)
PRJ_FWRAP(bon)
PRJ_FWRAP(pco)
PRJ_FWRAP(tsc)
PRJ_FWRAP(csc)
PRJ_FWRAP(qsc)
