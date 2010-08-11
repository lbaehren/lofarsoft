c! ra, dec --> x, y
c! ra, dec in radians

        subroutine radec2xy(ra,dec,x,y,error,ctype,crpix,cdelt,
     /             crval,crota)
        implicit none
        include "constants.inc"
        integer nchar,error,i
        real*8 x,y,ra,dec,arg,xintmd,yintmd,phi,theta
        real*8 crpix(3),cdelt(3),crval(3),crota(3),s
        character ctype(3)*8,ctype1(5)*8,ctype2(5)*8
        real*8 l,m,cosd0,sind0,t,da,ra1,dec1,sinrho,cosrho
        real*8 sina,cosa,sind,cosd
        logical iscoded(3)

        data ctype1/'RA---SIN','RA---TAN','RA---NCP','RA---CAR',
     /       'RA---FLT'/
        data ctype2/'DEC--SIN','DEC--TAN','DEC--NCP','DEC--CAR',
     /       'DEC--FLT'/
        error=0

c! TAN, SIN, NCP
        if (ctype(1)(6:8).eq.'SIN'.or.ctype(1)(6:8).eq.'TAN'.or.
     /      ctype(1)(6:8).eq.'NCP') then
         sinrho=dsin(crota(2)/rad)
         cosrho=dcos(crota(2)/rad)
         cosd0=dcos(crval(2)/rad)
         sind0=dsin(crval(2)/rad)
        end if

c! TAN from WJV - different stuff; all in radians
        if (ctype(1).eq.'RA---TAN'.and.ctype(2).eq.'DEC--TAN') then
         da=ra-crval(1)/rad  ! rad
         sina=dsin(da)
         cosa=dcos(da)
         sind=dsin(dec)
         cosd=dcos(dec)
         t=(sind*sind0+cosd*cosd0*cosa)
         l=cosd*sina/t
         m=(sind*cosd0-cosd*sind0*cosa)/t
         x=(l*cosrho+m*sinrho)*rad/cdelt(1)+crpix(1)
         y=(m*cosrho-l*sinrho)*rad/cdelt(2)+crpix(2)
        end if

c! SIN from WJV - different stuff; all in radians
        if (ctype(1).eq.'RA---SIN'.and.ctype(2).eq.'DEC--SIN') then
         da=ra-crval(1)/rad  ! rad
         cosd=dcos(dec)
         l=cosd*dsin(da)
         m=(dsin(dec)*cosd0)-(cosd*sind0*dcos(da))
         x=(l*cosrho+m*sinrho)*rad/cdelt(1)+crpix(1)
         y=(m*cosrho-l*sinrho)*rad/cdelt(2)+crpix(2)
        end if

c! now NCP from WJV - different stuff; all in radians
        if (ctype(1).eq.'RA---NCP'.and.ctype(2).eq.'DEC--NCP') then
         da=ra-crval(1)/rad
         cosd=dcos(dec)
         l=cosd*dsin(da)
         m=(cosd0-cosd*dcos(da))/sind0
         x=(l*cosrho+m*sinrho)*rad/cdelt(1)+crpix(1)
         y=(m*cosrho-l*sinrho)*rad/cdelt(2)+crpix(2)
        end if

c! CAR projection (for buddelmeijers image)
        if ((ctype(1).eq.'RA---FLT'.and.ctype(2).eq.'DEC--FLT').or.
     /     (ctype(1).eq.'RA---CAR'.and.ctype(2).eq.'DEC--CAR')) then
         x=(ra*rad-crval(1))/cdelt(1)+crpix(1)
         y=(dec*rad-crval(2))/cdelt(2)+crpix(2)
         x=(x*cosrho+y*sinrho)
         y=(y*cosrho-x*sinrho)
        end if

        iscoded(1)=.false.
        iscoded(2)=.false.
        do i=1,5
         iscoded(1)=iscoded(1).or.(ctype(1).eq.ctype1(i))
         iscoded(2)=iscoded(2).or.(ctype(2).eq.ctype2(i))
        end do
        if (.not.(iscoded(1).and.iscoded(2))) error=1

        return
        end
