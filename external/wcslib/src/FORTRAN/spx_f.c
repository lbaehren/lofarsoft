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
*   $Id: spx_f.c,v 4.2 2005/09/21 13:25:32 cal103 Exp $
*===========================================================================*/

#include <string.h>

#include <spx.h>

/*--------------------------------------------------------------------------*/

int specx_(
   const char *type,
   const double *spec,
   const double *restfrq,
   const double *restwav,
   double *specs)

{
   char stype[5];
   strncpy(stype, type, 4);
   stype[4] = '\0';
   return specx(stype, *spec, *restfrq, *restwav, (struct spxprm *)specs);
}

/*--------------------------------------------------------------------------*/

#define SPX_FWRAP(SPEC) \
   int SPEC##_( \
      const double *rest, \
      const int *n1, \
      const int *s1, \
      const int *s2, \
      const double spec1[], \
      double spec2[], \
      int stat[]) \
   {return SPEC(*rest, *n1, *s1, *s2, spec1, spec2, stat);}

SPX_FWRAP(freqafrq)
SPX_FWRAP(afrqfreq)

SPX_FWRAP(freqener)
SPX_FWRAP(enerfreq)

SPX_FWRAP(freqwavn)
SPX_FWRAP(wavnfreq)

SPX_FWRAP(freqvrad)
SPX_FWRAP(vradfreq)

SPX_FWRAP(freqwave)
SPX_FWRAP(wavefreq)

SPX_FWRAP(freqawav)
SPX_FWRAP(awavfreq)

SPX_FWRAP(freqvelo)
SPX_FWRAP(velofreq)

SPX_FWRAP(wavevopt)
SPX_FWRAP(voptwave)

SPX_FWRAP(wavezopt)
SPX_FWRAP(zoptwave)

SPX_FWRAP(waveawav)
SPX_FWRAP(awavwave)

SPX_FWRAP(wavevelo)
SPX_FWRAP(velowave)

SPX_FWRAP(awavvelo)
SPX_FWRAP(veloawav)

SPX_FWRAP(velobeta)
SPX_FWRAP(betavelo)
