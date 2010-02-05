c==============================================================================
      program getmjd
c==============================================================================
c
c     Returns, to the nearest second, the MJD at the time of
c     calling this routine by using the ship's clock.
c
c     Should be millenium proof but, quite frankly, I don't give a damn!
c
c     Creation date: 98/04/30 (dunc@mpifr-bonn.mpg.de)
c      
c==============================================================================
c      
      implicit none
      double precision mjdnow
      integer yy,mm,dd,hh,mi,ss,j
      call clock(yy,mm,dd,hh,mi,ss)
      if (yy.lt.1998) yy=yy+100 ! Millenium check
      call sla_cldj(yy,mm,dd,mjdnow,j)
      mjdnow=mjdnow+real(hh)/24.0+real(mi)/1440.0+real(ss)/86400.0
	write(*,*) mjdnow
      end
c==============================================================================
      SUBROUTINE sla_CLDJ (IY, IM, ID, DJM, J)
*+
*     - - - - -
*      C L D J
*     - - - - -
*
*  Gregorian Calendar to Modified Julian Date
*
*  Given:
*     IY,IM,ID     int    year, month, day in Gregorian calendar
*
*  Returned:
*     DJM          dp     modified Julian Date (JD-2400000.5) for 0 hrs
*     J            int    status:
*                           0 = OK
*                           1 = bad year   (MJD not computed)
*                           2 = bad month  (MJD not computed)
*                           3 = bad day    (MJD computed)
*
*  The year must be -4699 (i.e. 4700BC) or later.
*
*  The algorithm is derived from that of Hatcher 1984
*  (QJRAS 25, 53-55).
*
*  P.T.Wallace   Starlink   December 1985
*-

      IMPLICIT NONE

      INTEGER IY,IM,ID
      DOUBLE PRECISION DJM
      INTEGER J

*  Month lengths in days
      INTEGER MTAB(12)
      DATA MTAB/31,28,31,30,31,30,31,31,30,31,30,31/



*  Preset status
      J=0

*  Validate year
      IF (IY.LT.-4699) THEN
         J=1
      ELSE

*     Validate month
         IF (IM.GE.1.AND.IM.LE.12) THEN

*        Allow for leap year
            IF (MOD(IY,4).EQ.0) THEN
               MTAB(2)=29
            ELSE
               MTAB(2)=28
            END IF
            IF (MOD(IY,100).EQ.0.AND.MOD(IY,400).NE.0)
     :         MTAB(2)=28

*        Validate day
            IF (ID.LT.1.OR.ID.GT.MTAB(IM)) J=3

*        Modified Julian Date
               DJM=DBLE((1461*(IY-(12-IM)/10+4712))/4
     :                  +(306*MOD(IM+9,12)+5)/10
     :                  -(3*((IY-(12-IM)/10+4900)/100))/4
     :                  +ID-2399904)

*        Bad month
         ELSE
            J=2
         END IF

      END IF

      END
c==============================================================================
      subroutine clock(yy,mm,dd,hh,mi,ss)
c==============================================================================
c
c     Returns the present time from the Sun-OS clock
c      
c     yy - i4 - year   
c     mm - i4 - month
c     dd - i4 - day  
c     hh - i4 - hour
c     mi - i4 - minute
c     ss - i4 - second
c      
c     Creation date: 98/06/08 (dlorimer@naic.edu)
c      
c==============================================================================
c      
      implicit none
      integer yy,mm,dd,hh,mi,ss
      
      integer iarray(3)
      
      call idate(iarray)
      mm=iarray(2)
      dd=iarray(1)
      yy=iarray(3)
      call itime(iarray)
      hh=iarray(1)
      mi=iarray(2)
      ss=iarray(3)
      end
c
c==============================================================================

