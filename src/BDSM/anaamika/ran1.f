c! From NRP. Converted to real*8

c     ==================================================================
c     = Minimal random number generator of Park and Miller with Bays-  =
c     = Durham shuffle and added safeguards.                           =
c     = This routine returns randoam deviate between 0.0 and 1.0       =
c     = Ref: Numerical Recipes p-271                                   =
c     ==================================================================

      subroutine ran1(idum,ans)
      INTEGER idum, IA, IM, IQ, IR, NTAB, NDIV
      REAL*8  AM, EPS, RNMX,ans
      PARAMETER(IA=16807, IM=2147483647, AM=1./(IM*1.0), IQ=127773, 
     :     IR=2836, NTAB=32, NDIV=1 + (IM-1)/NTAB, EPS=1.2E-7, 
     :     RNMX=1.-EPS)

      INTEGER j, k, iv(NTAB), iy
      SAVE iv, iy
      DATA iv/NTAB*0/, iy/0/

      IF(idum.LE.0.OR.iy.EQ.0) THEN     !Initialize
       idum = max(-idum, 1)
       DO j=NTAB+8,1,-1
        k=idum/IQ
        idum = IA*(idum-k*IQ)-IR*k
        IF(idum.LT.0) idum= idum + IM
        IF(j .LE. NTAB) iv(j)=idum
       END DO
       iy=iv(1)
      END IF
      k= idum/IQ
      idum= IA * (idum-k*IQ)-IR*k
      IF(idum.LT.0) idum = idum +IM
      j = 1 +iy/NDIV
      iy = iv(j)
      iv (j) = idum
      ans = MIN(AM*DBLE(iy), RNMX)

      RETURN
      END
