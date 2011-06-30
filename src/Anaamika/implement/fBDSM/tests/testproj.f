c! test if wjv and my code for xy2radec is same

        implicit none
        real*8 crpix(3),cdelt(3),crval(3),crota(3)
        real*8 x,y,ra1,dec1,ra2,dec2
        character ctype(3)*8
        integer error,i,j

        ctype(1)='RA---SIN'
        ctype(2)='DEC--SIN'
        ctype(3)='FREQ    '
        crpix(1)=256.0
        crpix(2)=256.0
        crpix(3)=1.d0
        crval(1)=35.3245d0
        crval(2)=80.2345d0
        crval(3)=1.d6
        crota(1)=0.d0
        crota(2)=0.d0
        crota(3)=0.d0
        cdelt(1)=30.d0/3600.d0
        cdelt(2)=30.d0/3600.d0
        cdelt(3)=1.d5

        do i=1,800,50
         do j=1,800,50
          x=i*1.d0
          y=j*1.d0
          call wjv(x,y,ra1,dec1,error,ctype,crpix,cdelt,
     /             crval,crota)
          call mine(x,y,ra2,dec2,error,ctype,crpix,cdelt,
     /             crval,crota)
          if (i.eq.1.and.j.eq.1) write (*,*) ra1,dec1
          write (*,'(f9.7,2x,f9.7)') ra1-ra2,dec1-dec2
         end do
        end do
        write (*,*) ra1,dec1

        end

        subroutine wjv(x,y,ra1,dec1,error,ctype,crpix,cdelt,
     /             crval,crota)
        implicit none
        include "constants.inc"
        real*8 x,y,ra1,dec1,x1,y1,l,m,t,da,cosd0,sind0
        real*8 crpix(3),cdelt(3),crval(3),crota(3),s
        character ctype(3)*8
        integer error

        sind0 = dsin(crval(2)/rad)
        cosd0 = dcos(crval(2)/rad)
        
        x1=(x-crpix(1))*cdelt(1)
        y1=(y-crpix(2))*cdelt(2)
        l=x1/rad
        m=y1/rad

        t=sqrt(1.d0-l*l-m*m)   !  SIN
        da=atan(l/(cosd0*t-m*sind0))
        ra1=crval(1)/rad+da
        dec1=asin(m*cosd0+sind0*t)

        s=m*cosd0+sind0        ! TAN
        t=cosd0-m*sind0
        da=atan(l/t)
        ra1=crval(1)/rad+da
        dec1=atan(dcos(da)*s/t)

        ra1=ra1*rad
        dec1=dec1*rad

        return
        end


        subroutine mine(x,y,ra1,dec1,error,ctype,crpix,cdelt,
     /             crval,crota)
        implicit none
        include "constants.inc"
        real*8 x,y,ra1,dec1,xintmd,yintmd,uarg
        real*8 crpix(3),cdelt(3),crval(3),crota(3),phi,theta
        character ctype(3)*8
        integer error

        xintmd=cdelt(1)*(x-crpix(1))
        yintmd=cdelt(2)*(y-crpix(2))
c                                      all angles in radians
        phi=uarg(xintmd,-yintmd)
        theta=acos(pi*sqrt(xintmd*xintmd+yintmd*yintmd)/180.d0)  ! SIN
        theta=atan(180.d0/sqrt(xintmd*xintmd+yintmd*yintmd)/pi)  ! TAN

        dec1=asin(dsin(theta)*dsin(crval(2)/rad)-dcos(theta)*
     /            dcos(crval(2)/rad)*dcos(phi))                   ! rad
        ra1=asin(dcos(theta)*dsin(phi)/dcos(dec1))+crval(1)/rad    ! rad
        ra1=ra1*rad
        dec1=dec1*rad

        return
        end


