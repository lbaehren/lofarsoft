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
*   $Id: spc.inc,v 4.2 2005/09/21 13:25:32 cal103 Exp $
*=======================================================================

*     Functions.
      INTEGER   SPCGET, SPCINI, SPCPRT, SPCPUT, SPCS2X, SPCSET, SPCSPX,
     :          SPCTRN, SPCTYP, SPCX2S, SPCXPS

*     Length of the SPCPRM data structure (INTEGER array).
      INTEGER   SPCLEN
      PARAMETER (SPCLEN = 42)

*     Codes for SPC data structure elements used by SPCPUT and SPCGET.
      INTEGER   SPC_CODE, SPC_CRVAL, SPC_FLAG, SPC_PV, SPC_RESTFRQ,
     :          SPC_RESTWAV, SPC_TYPE

      PARAMETER (SPC_FLAG    = 100)
      PARAMETER (SPC_TYPE    = 101)
      PARAMETER (SPC_CODE    = 102)
      PARAMETER (SPC_CRVAL   = 103)
      PARAMETER (SPC_RESTFRQ = 104)
      PARAMETER (SPC_RESTWAV = 105)
      PARAMETER (SPC_PV      = 106)

*     Codes for SPC data structure elements used by SPCGET (only).
      INTEGER   SPC_ISGRISM, SPC_W

      PARAMETER (SPC_W       = 200)
      PARAMETER (SPC_ISGRISM = 201)

      CHARACTER SPC_ERRMSG(0:4)*80
      DATA SPC_ERRMSG /
     :   'Success',
     :   'Null spcprm pointer passed',
     :   'Invalid spectral parameters',
     :   'One or more of x coordinates were invalid',
     :   'One or more of the spec coordinates were invalid'/
