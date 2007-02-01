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
*   $Id: tpih2.f,v 4.2 2005/09/21 13:37:53 cal103 Exp $
*=======================================================================

      PROGRAM TPIH2
*-----------------------------------------------------------------------
*
*   TPIH2 tests WCSPIH, the WCS FITS parser for image headers, by
*   reading a test header and using PGSBOX to plot coordinate graticules
*   for the resulting WCSPRM structs.
*
*   Input comes from file 'test.fits'.
*
*-----------------------------------------------------------------------
      LOGICAL   GOTEND
      INTEGER   C0(7), GCODE(2), I, IC, IERR, J, K, NAXIS(2),
     :          NCARDS, NREJECT, NWCS, RELAX, WCSP
      REAL      BLC(2), TRC(2)
      DOUBLE PRECISION CACHE(4,0:256), NLDPRM(8)
      CHARACTER CARD*80, DEVTYP*16, HEADER*28801, IDENTS(3)*80,
     :          INFILE*9, NLCPRM(1)*1, OPT(2)*1, WCSNAME*72

      INCLUDE 'wcshdr.inc'
      INCLUDE 'wcs.inc'
      INTEGER WCS(WCSLEN)

      EXTERNAL PGWCSL

      DATA INFILE /'test.fits'/
      DATA C0     /7*-1/
*-----------------------------------------------------------------------
      WRITE (*, 10)
 10   FORMAT (
     :   'Testing WCSLIB parser for FITS image headers (tpih2.f)',/,
     :   '------------------------------------------------------',/)

*     Open the FITS WCS test header for formatted, direct I/O.
      OPEN (UNIT=1, FILE=INFILE, FORM='FORMATTED', ACCESS='DIRECT',
     :      RECL=80, IOSTAT=IERR)
      IF (IERR.NE.0) THEN
         WRITE (*, 20) IERR, INFILE
 20      FORMAT ('ERROR',I3,' opening ',A)
         GO TO 999
      END IF

*     Read in the header discarding COMMENT, &c. cards in the process.
      K = 1
      NCARDS = 0
      GOTEND = .FALSE.
      DO 50 J = 0, 100
         DO 40 I = 1, 36
            READ (1, '(A80)', REC=36*J+I, IOSTAT=IERR) CARD
            IF (IERR.NE.0) THEN
               WRITE (*, 30) IERR
 30            FORMAT ('ERROR',I3,' reading header.')
               GO TO 999
            END IF

            IF (CARD(:8).EQ.'        ') GO TO 40
            IF (CARD(:8).EQ.'COMMENT ') GO TO 40
            IF (CARD(:8).EQ.'HISTORY ') GO TO 40

            HEADER(K:) = CARD
            K = K + 80
            NCARDS = NCARDS + 1

            IF (CARD(:8).EQ.'END     ') THEN
*              An END card was read, but read the rest of the block.
               GOTEND = .TRUE.
            END IF
 40      CONTINUE

         IF (GOTEND) GO TO 60
 50   CONTINUE

 60   CLOSE (UNIT=1)

      HEADER(K:K) = CHAR (0)
      WRITE (*, 70) NCARDS
 70   FORMAT ('Found',I4,' non-comment header cards.')


*     WCSPIH will allocate memory for an array of NWCS intialized WCSPRM
*     structs.  This array will be returned via a "pointer", WCSP, that
*     may be passed to WCSVCOPY to extract a particular struct.
      RELAX  = WCSHDR_all
      IERR = WCSPIH (HEADER, NCARDS, RELAX, 3, NREJECT, NWCS, WCSP)
      IF (IERR.NE.0) THEN
         WRITE (*, 80) IERR
 80      FORMAT ('WCSPIH ERROR',I2,'.')
         GO TO 999
      END IF

*     Plot setup.
      NAXIS(1) = 1024
      NAXIS(2) = 1024

      BLC(1) = 0.5
      BLC(2) = 0.5
      TRC(1) = NAXIS(1) + 0.5
      TRC(2) = NAXIS(2) + 0.5

      DEVTYP = '/XWINDOW'
      CALL PGBEG(0, DEVTYP, 1, 1)
      CALL PGVSTD()

      CALL PGWNAD(0.0, 1.0, 0.0, 1.0)
      CALL PGASK(1)
      CALL PGPAGE()

*     Annotation.
      IDENTS(1) = 'Right ascension'
      IDENTS(2) = 'Declination'

      OPT(1) = 'G'
      OPT(2) = 'E'

*     Compact lettering.
      CALL PGSCH(0.8)

*     Draw full grid lines.
      CALL PGSCI(1)
      GCODE(1) = 2
      GCODE(2) = 2

      DO 100 I = 0, NWCS-1
*        Copy into our WCSPRM struct.
         IERR = WCSVCOPY (WCSP, I, WCS)

         IERR = WCSSET (WCS)
         IF (IERR.NE.0) THEN
           WRITE (*, 90) IERR
 90        FORMAT ('WCSSET ERROR',I2,'.')
           GO TO 999
         END IF

*        Get WCSNAME out of the WCSPRM struct.
         IERR = WCSGET (WCS, WCS_WCSNAME, WCSNAME)
         IDENTS(3) = WCSNAME
         WRITE (*, '(/,A)') WCSNAME

*        Draw the celestial grid.  The grid density is set for each
*        world coordinate by specifying LABDEN = 1224.
         IC = -1
         CALL PGSBOX(BLC, TRC, IDENTS, OPT, 0, 1224, C0, GCODE, 0D0, 0,
     :      0D0, 0, 0D0, 0, PGWCSL, 1, WCSLEN, 1, NLCPRM, WCS,
     :      NLDPRM, 256, IC, CACHE, IERR)

*        Draw the frame.
         CALL PGBOX('BC', 0.0, 0, 'BC', 0.0, 0)
         CALL PGPAGE()

*        Free memory (doesn't free memory allocated by WCSPIH).
         IERR = WCSFREE (WCS)
 100  CONTINUE

*     Free the memory allocated by WCSPIH.
      IERR = WCSVFREE (NWCS, WCSP)

 999  CONTINUE
      END
