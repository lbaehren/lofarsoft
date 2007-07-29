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
*   $Id: tpih1.c,v 4.2 2005/09/23 03:09:34 cal103 Exp $
*=============================================================================
*
*   tpih1 tests wcspih(), the WCS FITS parser for image headers, and wcsfix(),
*   which translates non-standard constructs.  It reads a test header and uses
*   wcsprt() to print the resulting wcsprm structs.
*
*   Input comes from file "test.fits" using either fits_hdr2str() from CFITSIO
*   if the DO_CFITSIO preprocessor is defined, or read directly using fgets()
*   otherwise.
*
*---------------------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>

#if defined HAVE_CFITSIO && defined DO_CFITSIO
   #include <fitsio.h>
#endif

#include <wcs.h>
#include <wcshdr.h>
#include <wcsfix.h>

int main()

{
   char infile[] = "test.fits";
   char a, *hptr;
   int  alts[27], ctrl, i, j, ncards, nreject, nwcs, relax, stat[NWCSFIX],
        status;
   struct wcsprm *wcs;
#if defined HAVE_CFITSIO && defined DO_CFITSIO
   char *header;
   fitsfile *fptr;
#else
   char card[81], header[288001];
   int  gotend, k;
   FILE *fptr;
#endif

   printf("Testing WCSLIB parser for FITS image headers (tpih1.c)\n"
          "------------------------------------------------------\n\n");
#if defined HAVE_CFITSIO && defined DO_CFITSIO
   /* Read in the FITS header discarding COMMENT, &c. cards in the process. */

   status = 0;

   if (fits_open_file(&fptr, infile, READONLY, &status)) {
      fits_report_error(stderr, status);
      return 1;
   }

   if (fits_hdr2str(fptr, 1, NULL, 0, &header, &ncards, &status)) {
      fits_report_error(stderr, status);
      return 1;
   }

   fits_close_file(fptr, &status);
#else
   if ((fptr = fopen(infile, "r")) == 0) {
      fprintf(stderr, "ERROR opening %s\n", infile);
      return 1;
   }

   k = 0;
   ncards = 0;
   gotend = 0;
   for (j = 0; j < 100; j++) {
      for (i = 0; i < 36; i++) {
         if (fgets(card, 81, fptr) == 0) {
            break;
         }

         if (strncmp(card, "        ", 8) == 0) continue;
         if (strncmp(card, "COMMENT ", 8) == 0) continue;
         if (strncmp(card, "HISTORY ", 8) == 0) continue;

         strncpy(header+k, card, 80);
         k += 80;
         ncards++;

         if (strncmp(card, "END     ", 8) == 0) {
            /* An END card was read, but read the rest of the block. */
            gotend = 1;
         }
      }

      if (gotend) break;
   }
   fclose(fptr);
#endif

   fprintf(stderr, "Found %d non-comment header cards.\n\n", ncards);

   /* Cull all WCS cards from the header but report illegal ones. */
   printf("\nIllegal-WCS header cards rejected by wcspih():\n");
   relax = WCSHDR_all;
   ctrl = -2;
   if (status = wcspih(header, ncards, relax, ctrl, &nreject, &nwcs, &wcs)) {
      fprintf(stderr, "wcspih ERROR %d: %s.\n", status, wcs_errmsg[status]);
   }
   if (!nreject) fprintf(stderr, "(none)\n");

   /* List cards that were not consumed by wcspih(). */
   printf("\n\nNon-WCS header cards not used by wcspih():\n");
   hptr = header;
   while (*hptr) {
      printf("%.80s\n", hptr);
      hptr += 80;
   }
#if defined HAVE_CFITSIO && defined DO_CFITSIO
   free(header);
#endif

   status = wcsidx(nwcs, &wcs, alts);
   printf("\n\nIndex of alternate coordinate descriptions found:\n  ");
   for (a = 'A'; a <= 'Z'; a++) {
      printf("%2c", a);
   }
   printf("\n");
   for (i = 0; i < 27; i++) {
      if (alts[i] < 0) {
         printf(" -");
      } else {
         printf("%2d", alts[i]);
      }
   }
   printf("\n");

   for (i = 0; i < nwcs; i++) {
      printf("\n------------------------------------"
             "------------------------------------\n");

      /* Fix non-standard WCS keyvalues. */
      if (status = wcsfix(7, 0, wcs+i, stat)) {
         printf("wcsfix ERROR, status returns: (");
         for (j = 0; j < NWCSFIX; j++) {
            printf(j ? ", %d" : "%d", stat[j]);
         }
         printf(")\n\n");
      }

      if (status = wcsset(wcs+i)) {
         fprintf(stderr, "wcsset ERROR %d: %s.\n", status,
                 wcs_errmsg[status]);
         continue;
      }

      if (status = wcsprt(wcs+i)) {
         fprintf(stderr, "wcsprt ERROR %d: %s.\n", status,
                 wcs_errmsg[status]);
      }
   }

   status = wcsvfree(&nwcs, &wcs);

   return 0;
}
