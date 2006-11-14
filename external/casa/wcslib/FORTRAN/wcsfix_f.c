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
*   $Id: wcsfix_f.c,v 4.2 2005/09/21 13:25:32 cal103 Exp $
*===========================================================================*/

#include <wcsfix.h>

/*--------------------------------------------------------------------------*/

int wcsfix_(int *ctrl, const int naxis[], int *wcs, int stat[])

{
   return wcsfix(*ctrl, naxis, (struct wcsprm *)wcs, stat);
}

/*--------------------------------------------------------------------------*/

int datfix_(int *wcs)

{
   return datfix((struct wcsprm *)wcs);
}

/*--------------------------------------------------------------------------*/

int unitfix_(int *ctrl, int *wcs)

{
   return unitfix(*ctrl, (struct wcsprm *)wcs);
}

/*--------------------------------------------------------------------------*/

int celfix_(int *wcs)

{
   return celfix((struct wcsprm *)wcs);
}

/*--------------------------------------------------------------------------*/

int spcfix_(int *wcs)

{
   return spcfix((struct wcsprm *)wcs);
}

/*--------------------------------------------------------------------------*/

int cylfix_(const int naxis[], int *wcs)

{
   return cylfix(naxis, (struct wcsprm *)wcs);
}
