/*============================================================================
*
*   WCSLIB 4.2 - an implementation of the FITS WCS standard.
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
*   $Id: getwcstab.h,v 4.2 2005/09/21 13:21:57 cal103 Exp $
*=============================================================================
*
*   IMPORTANT
*   ---------
*   The interface of fits_read_wcstab() is experimental, getwcstab.{h,c} must
*   be considered beta-release code in WCSLIB 4.2.  The code will eventually
*   be incorporated into CFITSIO with the definitions in getwcstab.h moved
*   into fitsio.h.
*
*
*   FITS -TAB table reading routine; fits_read_wcstab()
*   ---------------------------------------------------
*   fits_read_wcstab() extracts arrays from a binary table required in
*   constructing -TAB coordinates.
*
*   This implementation is provided for CFITSIO programmers, but others should
*   find the code in getwcstab.c useful as a programming template.
*
*   Given:
*      fptr     fitsfile *
*                        Pointer to the file handle returned, for example, by
*                        the fits_open_file() routine in CFITSIO.
*
*      nwtb     int      Number of arrays to be read from the binary table(s).
*
*   Given and returned:
*      wtb      wtbarr *
*                        Address of the first element of an array of wtbarr
*                        typedefs.  This wtbarr typedef is defined below to
*                        match the wtbarr struct defined in WCSLIB.  An array
*                        of such structs returned by the WCSLIB function
*                        wcstab().  See note 1 below.
*
*   Returned:
*      status   int *    CFITSIO status value.
*
*   Function return value:
*               int      CFITSIO status value.
*
*
*   Notes
*   -----
*    1) In order to maintain WCSLIB and CFITSIO as independent libraries it
*       was not permissible for any CFITSIO library code to include WCSLIB
*       header files, or vice versa.  However, the CFITSIO function
*       fits_read_wcstab() accepts an array of structs defined by wcs.h within
*       WCSLIB.  The problem then was to define this struct within fitsio.h
*       without including wcs.h, especially noting that wcs.h will often (but
*       not always) be included together with fitsio.h in an applications
*       program that uses fits_read_wcstab().
*
*       Of the various possibilities, the solution adopted was for WCSLIB to
*       define "struct wtbarr" while fitsio.h defines "typedef wtbarr", a
*       untagged struct with identical members.  This allows both wcs.h and
*       fitsio.h to define a wtbarr data type without conflict by virtue of
*       the fact that structure tags and typedef names share different
*       namespaces in C.  Appendix A, Sect. 11.1 (p206) of the K&R ANSI
*       edition states that:
*
*          ... identifiers associated with ordinary variables on the one hand
*          and those associated with structure and union members and tags on
*          the other form two disjoint classes which do not conflict.  [...]
*          typedef names are in the same class as ordinary identifiers. [...]
*
*       Therefore, declarations within WCSLIB look like
*
*          struct wtbarr *w;
*
*       while within CFITSIO they are simply
*
*          wtbarr *w;
*
*       but as suggested by the commonality of the names, these are really the
*       same aggregate data type.  However, in passing a (struct wtbarr *) to
*       fits_read_wcstab() a cast to (wtbarr *) is formally required.
*
*
*   wtbarr typedef
*   --------------
*   The wtbarr typedef is defined as a struct containing the following
*   members:
*
*      int i
*         Image axis number.
*
*      int m
*         Array axis number for index vectors.
*
*      int kind
*         Character identifying the array type:
*            'c': coordinate array,
*            'i': index vector.
*
*      char extnam[72]
*         EXTNAME identifying the binary table extension.
*
*      int extver
*         EXTVER identifying the binary table extension.
*
*      int extlev
*         EXTLEV identifying the binary table extension.
*
*      char ttype[72]
*         TTYPEn identifying the column of the binary table that contains the
*         array.
*
*      long row
*         Table row number.
*
*      int ndim
*         Expected dimensionality of the array.
*
*      int *dimlen
*         Address of the first element of an array of int of length ndim into
*         which the array axis lengths are to be written.
*
*      double **arrayp
*         Pointer to an array of double which is to be allocated by the user
*         and into which the array is to be written.
*
*===========================================================================*/

#ifndef WCSLIB_GETWCSTAB
#define WCSLIB_GETWCSTAB

#ifdef __cplusplus
extern "C" {
#endif

#include <fitsio.h>

typedef struct {
   int  i;			/* Image axis number.                       */
   int  m;			/* Array axis number for index vectors.     */
   int  kind;			/* Array type, 'c' (coord) or 'i' (index).  */
   char extnam[72];		/* EXTNAME of binary table extension.       */
   int  extver;			/* EXTVER  of binary table extension.       */
   int  extlev;			/* EXTLEV  of binary table extension.       */
   char ttype[72];		/* TTYPEn of column containing the array.   */
   long row;			/* Table row number.                        */
   int  ndim;			/* Expected array dimensionality.           */
   int  *dimlen;		/* Where to write the array axis lengths.   */
   double **arrayp;		/* Where to write the address of the array  */
                                /* allocated to store the array.            */
} wtbarr;

int fits_read_wcstab(fitsfile *, int, wtbarr *, int *);

#ifdef __cplusplus
};
#endif

#endif /* WCSLIB_GETWCSTAB */
