c! this is temporary before i put in wcslib. converts x,y to ra,dec for TAN only
c! all angles in radians

        subroutine xy2radec(x,y,ra,dec,error,ctype,crpix,cdelt,
     /             crval,crota)
        implicit none
<<<<<<< HEAD
        include "constants.inc"
=======
        include "includes/constants.inc"
>>>>>>> Updating source file list for noise; cleanining up include statements, as we no longer allow for symbolic links inside the source directory.
        integer nchar,error,i
        real*8 x,y,ra,dec,uarg,xintmd,yintmd,phi,theta
        real*8 crpix(3),cdelt(3),crval(3),crota(3),s
        character ctype(3)*8,ctype1(5)*8,ctype2(5)*8
        real*8 l,m,cosd0,sind0,t,da,ra1,dec1,sinrho,cosrho
        logical iscoded(3)

        data ctype1/'RA---SIN','RA---TAN','RA---NCP','RA---CAR',
     /       'RA---FLT'/
        data ctype2/'DEC--SIN','DEC--TAN','DEC--NCP','DEC--CAR',
     /       'DEC--FLT'/

        error=0

cc! SIN projection (WJV)
c        if (crota(1).eq.0.d0.and.crota(2).eq.0.d0) then
c         xintmd=cdelt(1)*(x-crpix(1))
c         yintmd=cdelt(2)*(y-crpix(2))
cc                                      all angles in radians
c         phi=uarg(xintmd,-yintmd)
c         if (ctype(1).eq.'RA---SIN'.and.ctype(2).eq.'DEC--SIN') 
c     /    theta=acos(pi*sqrt(xintmd*xintmd+yintmd*yintmd)/180.d0)
c         if (ctype(1).eq.'RA---TAN'.and.ctype(2).eq.'DEC--TAN') 
c     /    theta=atan(180.d0/sqrt(xintmd*xintmd+yintmd*yintmd)/pi)
c
c         dec=asin(dsin(theta)*dsin(crval(2)/rad)-dcos(theta)*
c     /            dcos(crval(2)/rad)*dcos(phi))                   ! rad
c         ra=asin(dcos(theta)*dsin(phi)/dcos(dec))+crval(1)/rad    ! rad
cc         write (*,*) ra-crval(1)/rad,ra*rad/15,' h'
c        end if

c! TAN, SIN, NCP

        if (ctype(1)(6:8).eq.'SIN'.or.ctype(1)(6:8).eq.'TAN'.or.
     /      ctype(1)(6:8).eq.'NCP') then
         sinrho=dsin(crota(2)/rad)
         cosrho=dcos(crota(2)/rad)
         xintmd=cdelt(1)*(x-crpix(1))
         yintmd=cdelt(2)*(y-crpix(2))
         l=(xintmd*cosrho-yintmd*sinrho)/rad
         m=(yintmd*cosrho+xintmd*sinrho)/rad
         cosd0=dcos(crval(2)/rad)
         sind0=dsin(crval(2)/rad)
        end if

c! TAN from WJV - different stuff; all in radians
        if (ctype(1).eq.'RA---TAN'.and.ctype(2).eq.'DEC--TAN') then
         s=m*cosd0+sind0
         t=cosd0-m*sind0
         da=atan(l/t)
         ra=crval(1)/rad+da
         dec=atan(dcos(da)*s/t)
        end if

c! SIN from WJV - different stuff; all in radians
        if (ctype(1).eq.'RA---SIN'.and.ctype(2).eq.'DEC--SIN') then
         t=sqrt(1.d0-l*l-m*m)
         da=atan(l/(t*cosd0-m*sind0))
         ra=crval(1)/rad+da
c         write (*,*) l/(t*cosd0-m*sind0),da,ra*rad/15,' h'
         dec=asin(m*cosd0+t*sind0)
        end if

c! now NCP from WJV - different stuff; all in radians
        if (ctype(1).eq.'RA---NCP'.and.ctype(2).eq.'DEC--NCP') then
         da=atan(l/(cosd0-m*sind0))
         dec=acos((cosd0-m*sind0)/dcos(da))
         if (crval(2).gt.0.0) then
          dec=dabs(dec)
         else
          dec=dabs(dec)*(-1.d0)
         end if
         ra=crval(1)/rad+da
        end if

c! CAR projection (for buddelmeijers image)
        if ((ctype(1).eq.'RA---FLT'.and.ctype(2).eq.'DEC--FLT').or.
     /     (ctype(1).eq.'RA---CAR'.and.ctype(2).eq.'DEC--CAR')) then
         ra=crval(1)/rad+l
         dec=crval(2)/rad+m
        end if

c!
        iscoded(1)=.false.
        iscoded(2)=.false.
        do i=1,5
         iscoded(1)=iscoded(1).or.(ctype(1).eq.ctype1(i))
         iscoded(2)=iscoded(2).or.(ctype(2).eq.ctype2(i))
        end do
        if (.not.(iscoded(1).and.iscoded(2))) error=1

        return
        end
