*=======================================================================
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
*   $Id: lin.inc,v 4.2 2005/09/21 13:25:32 cal103 Exp $
*=======================================================================

*     Functions.
      INTEGER   LININI, LINCPY, LINPUT, LINGET, LINFREE, LINPRT, LINSET,
     :          LINP2X, LINX2P

*     Length of the LINPRM data structure (INTEGER array).
      INTEGER   LINLEN
      PARAMETER (LINLEN = 14)

*     Codes for LIN data structure elements used by LINPUT and LINGET.
      INTEGER   LIN_CDELT, LIN_CRPIX, LIN_FLAG, LIN_NAXIS, LIN_PC

      PARAMETER (LIN_FLAG   = 100)
      PARAMETER (LIN_NAXIS  = 101)
      PARAMETER (LIN_CRPIX  = 102)
      PARAMETER (LIN_PC     = 103)
      PARAMETER (LIN_CDELT  = 104)

*     Codes for LIN data structure elements used by LINGET (only).
      INTEGER   LIN_IMGPIX, LIN_PIXIMG, LIN_UNITY

      PARAMETER (LIN_UNITY  = 200)
      PARAMETER (LIN_PIXIMG = 201)
      PARAMETER (LIN_IMGPIX = 202)

      CHARACTER LIN_ERRMSG(0:3)*80
      DATA LIN_ERRMSG /
     :   'Success',
     :   'Null linprm pointer passed',
     :   'Memory allocation failed',
     :   'PCi_ja matrix is singular'/
