c! this is temporary before i put in wcslib. converts x,y to ra,dec for TAN only
c! all angles in radians

        subroutine xy2radec(x,y,ra,dec,error,ctype,crpix,cdelt,
     /             crval,crota)
        implicit none
        include "constants.inc"
        integer nchar,error
        real*8 x,y,ra,dec,arg,xintmd,yintmd,phi,theta
        real*8 crpix(3),cdelt(3),crval(3),crota(3)
        character ctype(3)*8
        real*8 l,m,cosd0,sind0,t,da,ra1,dec1

        error=0

c! SIN projection (WJV)
        if (crota(1).eq.0.d0.and.crota(2).eq.0.d0) then
         xintmd=cdelt(1)*(x-crpix(1))
         yintmd=cdelt(2)*(y-crpix(2))
c                                      all angles in radians
         phi=arg(xintmd,-yintmd)
         if (ctype(1).eq.'RA---SIN'.and.ctype(2).eq.'DEC--SIN') 
     /    theta=acos(pi*sqrt(xintmd*xintmd+yintmd*yintmd)/180.d0)
         if (ctype(1).eq.'RA---TAN'.and.ctype(2).eq.'DEC--TAN') 
     /    theta=atan(180.d0/sqrt(xintmd*xintmd+yintmd*yintmd)/pi)

         if (ctype(1).eq.'RA---NCP'.and.ctype(2).eq.'DEC--NCP') then ! wrong
          theta=acos(pi*sqrt(xintmd*xintmd+yintmd*yintmd)/180.d0)
         end if

         dec=asin(dsin(theta)*dsin(crval(2)/rad)-dcos(theta)*
     /            dcos(crval(2)/rad)*dcos(phi))                   ! rad
         ra=asin(dcos(theta)*dsin(phi)/dcos(dec))+crval(1)/rad    ! rad
        end if

c! now NCP from WJV - different stuff; all in radians
        if (crota(1).eq.0.d0.and.crota(2).eq.0.d0.and.
     /      ctype(1).eq.'RA---NCP'.and.ctype(2).eq.'DEC--NCP') then
         xintmd=cdelt(1)*(x-crpix(1))
         yintmd=cdelt(2)*(y-crpix(2))
         l=xintmd/rad
         m=yintmd/rad
         cosd0=dcos(crval(2)/rad)
         sind0=dsin(crval(2)/rad)
         da=atan(l/(cosd0-m*sind0))
         dec=acos((cosd0-m*sind0)/dcos(da))
         if (crval(2).gt.0.0) then
          dec=dabs(dec)
         else
          dec=dabs(dec)*(-1.d0)
         end if
         ra=crval(1)/rad+da
        end if

        if (crota(1).ne.0.d0.or.crota(2).ne.0.d0.or.(ctype(1).ne.
     /    'RA---SIN'.and.ctype(1).ne.'RA---TAN'
     /    .and.ctype(1).ne.'RA---NCP').or.(ctype(2).ne.
     /    'DEC--SIN'.and.ctype(2).ne.'DEC--TAN'
     /    .and.ctype(2).ne.'DEC--NCP')) error=1

        return
        end


c! ---------------------- SUBROUTINES ----------------------------------
c! to initialise
        subroutine read_dum_head(n,ctype,crpix,cdelt,crval,crota,bm_pix)
        implicit none
        integer n
        real*8 crpix(n),cdelt(n),crval(n),crota(n),bm_pix(3)
        character ctype(n)*8

        ctype(1)='RA---SIN'
        ctype(2)='DEC--SIN'
        crpix(1)=1.d0
        crpix(2)=1.d0
        cdelt(1)=1.d0/3600.d0  ! deg
        cdelt(2)=1.d0/3600.d0  ! deg
        crval(1)=1.5d0*15.d0   ! deg
        crval(2)=10.5d0        ! deg
        crota(1)=0.d0
        crota(2)=0.d0
        bm_pix(1)=4.d0
        bm_pix(2)=4.d0
        bm_pix(3)=0.d0
        ctype(3)='FREQ'
        crpix(3)=1.d0
        cdelt(3)=4.d6
        crval(3)=120.d6
        crota(3)=0.d0

        return
        end

