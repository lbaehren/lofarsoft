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
*   $Id: tpih1.f,v 4.2 2005/09/21 13:37:53 cal103 Exp $
*=======================================================================

      PROGRAM TPIH1
*-----------------------------------------------------------------------
*
*   TPIH1 tests WCSPIH, the WCS FITS parser for image headers, and
*   WCSFIX, which translates non-standard constructs.  It reads a test
*   header and uses WCSPRT to print the resulting WCSPRM structs.
*
*   Input comes from file 'test.fits'.
*
*-----------------------------------------------------------------------
      LOGICAL   GOTEND
      INTEGER   ALTS(0:26), CTRL, I, IERR, J, K, NCARDS, NREJECT, NWCS,
     :          RELAX, WCSP
      CHARACTER CALTS(0:26)*2, CARD*80, HEADER*288001, INFILE*9

      INCLUDE 'wcshdr.inc'
      INCLUDE 'wcs.inc'
      INCLUDE 'wcsfix.inc'
      INTEGER WCS(WCSLEN), STAT(WCSFIX_NWCS)

      DATA INFILE /'test.fits'/
*-----------------------------------------------------------------------
      WRITE (*, 10)
 10   FORMAT (
     :   'Testing WCSLIB parser for FITS image headers (tpih1.f)',/,
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
 70   FORMAT ('Found',I4,' non-comment header cards.',/)


*     Cull all WCS cards from the header but report illegal ones.
      WRITE (*, 80)
 80   FORMAT (/,'Illegal-WCS header cards rejected by wcspih():')
      RELAX = WCSHDR_all
      CTRL = -2
*     WCSPIH will allocate memory for NWCS intialized WCSPRM structs.
      IERR = WCSPIH (HEADER, NCARDS, RELAX, CTRL, NREJECT, NWCS, WCSP)
      IF (IERR.NE.0) THEN
         WRITE (*, 90) IERR
 90      FORMAT ('WCSPIH ERROR',I2,'.')
         GO TO 999
      END IF

*     List cards that were not consumed by WCSPIH.
      WRITE (*, 100)
 100  FORMAT (//,'Non-WCS header cards not used by WCSPIH:')
      DO 110 I = 1, 288001, 80
         IF (HEADER(I:I).EQ.CHAR(0)) GO TO 120
         WRITE (*, '(A)') HEADER(I:I+79)
 110  CONTINUE

 120  IERR = WCSIDX (NWCS, WCSP, ALTS)
      WRITE (*, 130)
 130  FORMAT (//,'Index of alternate coordinate descriptions found:',/,
     :        '   A B C D E F G H I J K L M N O P Q R S T U V W X Y Z')
      DO 140 I = 0, 26
         IF (ALTS(I).LT.0) THEN
            CALTS(I) = ' -'
         ELSE
            WRITE (CALTS(I), '(I2)') ALTS(I)
         END IF
 140  CONTINUE
      WRITE (*, '(27A)') CALTS

      DO 190 I = 0, NWCS-1
         WRITE (*, 150)
 150     FORMAT (/,'------------------------------------',
     :             '------------------------------------')

*        Copy into our WCSPRM struct.
         IERR = WCSVCOPY (WCSP, I, WCS)

*        Fix non-standard WCS keyvalues.
         IERR = WCSFIX (7, 0, WCS, STAT)
         IF (IERR.NE.0) THEN
            WRITE (*, 160) (STAT(J), J=1,WCSFIX_NWCS)
 160        FORMAT ($,'WCSFIX ERROR, status returns: (',(I2,:,','),')',
     :              /)
         END IF

         IERR = WCSSET (WCS)
         IF (IERR.NE.0) THEN
           WRITE (*, 170) IERR
 170       FORMAT ('WCSSET ERROR',I2,'.')
           GO TO 190
         END IF

         IERR = WCSPRT (WCS)
         IF (IERR.NE.0) THEN
           WRITE (*, 180) IERR
 180       FORMAT ('WCSPRT ERROR',I2,'.')
           GO TO 190
         END IF

*        Free memory (doesn't free memory allocated by WCSPIH).
         IERR = WCSFREE (WCS)
 190  CONTINUE

*     Free the memory allocated by WCSPIH.
      IERR = WCSVFREE (NWCS, WCSP)

 999  CONTINUE
      END
