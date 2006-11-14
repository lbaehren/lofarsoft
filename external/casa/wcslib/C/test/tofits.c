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
*   $Id: tofits.c,v 4.2 2005/09/21 13:34:06 cal103 Exp $
*=============================================================================
*
*   tofits turns a list of FITS header cards, one per line, into a proper
*   FITS header by padding them with blanks to 80 characters and stripping
*   out newline characters.  It also pads the header to an integral number
*   of 2880-byte blocks if necessary.
*
*   It operates as a filter, e.g.:
*
*       tofits < infile > outfile
*
*===========================================================================*/

#include <stdio.h>

int main()

{
  int c, i = 0, ncard = 0;

  while ((c = getchar()) != EOF) {
    if (c == '\n') {
      /* Blank-fill the card. */
      while (i++ < 80) {
        putchar(' ');
      }
      i = 0;
      ncard++;

    } else {
      putchar(c);
      i++;
    }
  }

  /* Pad to a multiple of 2880-bytes. */
  if (ncard %= 36) {
    while (ncard++ < 36) {
      i = 0;
      while (i++ < 80) {
        putchar(' ');
      }
    }
  }

  return 0;
}
