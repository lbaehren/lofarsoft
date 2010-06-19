c! modified from twcs.f of wcslib test program.

        subroutine wcs_struct(n,ctypen,crpixn,cdeltn,crvaln,crotan,
     /             wcs,wcslen)
        implicit none
        include 'wcs_bdsm.inc'
        include 'cel.inc'
        include 'prj.inc'
        include 'constants.inc'
        integer n,wcslen,wcs(wcslen),i,j,status,naxis,altlin
        character ctypen(n)*8,dumc
        real*8 crpixn(n),cdeltn(n),crvaln(n),crotan(n),latpole,lonpole
        real*8 pc(2,2),crot
        character ctype(2)*72
        real*8 crpix(2),cdelt(2),crval(2),crota(2)

        dumc='n'
        NAXIS=2
        do i=1,NAXIS
         do j=1,NAXIS
          if (i.eq.j) then
           pc(i,j)=1.d0
          else
           pc(i,j)=0.d0
          end if
         end do
         ctype(i)=ctypen(i)
         cdelt(i)=cdeltn(i)
         crval(i)=crvaln(i)
         crpix(i)=crpixn(i)
         crota(i)=crotan(i)
        end do

*-----------------------------------------------------------------------
        STATUS = WCSPUT (WCS, WCS_FLAG, -1, 0, 0)

        STATUS = WCSINI (NAXIS, WCS)
  
        DO 20 I = 1, NAXIS
         STATUS = WCSPUT (WCS, WCS_CRPIX, CRPIX(I), I, 0)

         DO 10 J = 1, NAXIS
           STATUS = WCSPUT (WCS, WCS_PC, PC(I,J), I, J)
10       CONTINUE
         STATUS = WCSPUT (WCS, WCS_CDELT, CDELT(I), I, 0)
         STATUS = WCSPUT (WCS, WCS_CTYPE, CTYPE(I), I, 0)
         STATUS = WCSPUT (WCS, WCS_CRVAL, CRVAL(I), I, 0)
20      CONTINUE

        if (CTYPE(1)(1:3).eq.'DEC'.and.CRVAL(1).eq.90.d0.or.
     /     (CTYPE(2)(1:3).eq.'DEC'.and.CRVAL(2).eq.90.d0)) then
         LONPOLE=180.d0  
         LONPOLE=0.d0  ! works for sarod's 2007 image sarod2007I.fits
         STATUS = WCSPUT (WCS, WCS_LONPOLE, LONPOLE, 0, 0)
        end if

        if (crota(1).ne.0.d0.or.crota(2).ne.0.d0) then
         ALTLIN=4
         STATUS = WCSPUT (WCS, WCS_ALTLIN, ALTLIN, 0, 0)
         DO I = 1, NAXIS
          STATUS = WCSPUT (WCS, WCS_CROTA, CROTA(I), I, 0)
         END DO
        end if
        
  
        STATUS = WCSSET (WCS)
        IF (STATUS.NE.0) THEN
         WRITE (*, 40) STATUS
40       FORMAT ('WCSSET ERROR',I3)
        END IF
  
        RETURN
        END

