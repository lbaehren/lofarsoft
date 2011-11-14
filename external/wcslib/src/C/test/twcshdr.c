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
*   $Id: twcshdr.c,v 4.2 2005/09/22 02:27:33 cal103 Exp $
*=============================================================================
*
*   twcstab tests wcstab() and also provides sample code for using it in
*   conjunction with wcspih() and fits_read_wcstab().  Although this example
*   and fits_read_wcstab() are based on the CFITSIO library, wcstab() itself
*   is completely independent of it.
*
*   Input comes from a user-specified FITS file.
*
*===========================================================================*/

#include <stdio.h>

#include <fitsio.h>

#include <wcslib.h>
#include <getwcstab.h>

int main(int argc, char *argv[])

{
   char *header;
   int  i, ncard, nreject, nwcs, stat[NWCSFIX], status = 0;
   fitsfile *fptr;
   struct wcsprm *wcs;


   if (argc < 2) {
      fprintf(stderr, "Usage: twcshdr <file>\n");
      return 1;
   }

   /* Open the FITS test file and read the primary header. */
   fits_open_file(&fptr, argv[1], READONLY, &status);
   if (status = fits_hdr2str(fptr, 1, NULL, 0, &header, &ncard, &status)) {
      fits_report_error(stderr, status);
      return 1;
   }


   /*-----------------------------------------------------------------------*/
   /* Basic steps required to interpret a FITS WCS header, including -TAB.  */
   /*-----------------------------------------------------------------------*/

   /* Parse the primary header of the FITS file. */
   if (status = wcspih(header, ncard, WCSHDR_all, 2, &nreject, &nwcs, &wcs)) {
      fprintf(stderr, "wcspih ERROR %d: %s.\n", status,wcshdr_errmsg[status]);
   }

   /* Read coordinate arrays from the binary table extension. */
   if (status = fits_read_wcstab(fptr, wcs->nwtb, (wtbarr *)wcs->wtb,
       &status)) {
      fits_report_error(stderr, status);
      return 1;
   }

   /* Translate non-standard WCS keyvalues. */
   if (status = wcsfix(7, 0, wcs, stat)) {
      for (i = 0; i < NWCSFIX; i++) {
         if (stat[i] > 0) {
            fprintf(stderr, "wcsfix ERROR %d: %s.\n", status,
                    wcsfix_errmsg[stat[i]]);
         }
      }

      return 1;
   }

   /*-----------------------------------------------------------------------*/
   /* The wcsprm struct is now ready for use.                               */
   /*-----------------------------------------------------------------------*/

   /* Finished with the FITS file. */
   fits_close_file(fptr, &status);
   free(header);

   /* Initialize the wcsprm struct, also taking control of memory allocated by
    * fits_read_wcstab(). */
   if (status = wcsset(wcs)) {
      fprintf(stderr, "wcsset ERROR %d: %s.\n", status, wcs_errmsg[status]);
      return 1;
   }

   /* Print the struct. */
   if (status = wcsprt(wcs)) {
      return 1;
   }

   /* Clean up. */
   status = wcsvfree(&nwcs, &wcs);

   return 0;
}
