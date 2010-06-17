c! dirn of north anticlockwise from vertical in deg
c! see paplot.m
        
        subroutine angle_local_north(xmax,ymax,wcslen,wcs,
     /             cdelt,ang_n,nn) ! deg
        implicit none
        include "constants.inc"
        integer wcslen,wcs(wcslen),xmax,ymax,error1,nn
        real*8 ang_n,ramax,decmax,decl,decu,cdelt(nn),x1,y1,x2,y2
        real*8 dx,dy
        
        call wcs_xy2radec(xmax*1.d0,ymax*1.d0,ramax,decmax,error1,
     /       wcs,wcslen)     ! calc at max pixel, coord in rad
        decl=decmax-abs(cdelt(2))*0.3d0/rad    ! 0.3 pixels in dec
        decu=decmax+abs(cdelt(2))*0.3d0/rad    ! 0.3 pixels in dec
        call wcs_radec2xy(ramax,decl,x1,y1,error1,wcs,wcslen)
        call wcs_radec2xy(ramax,decu,x2,y2,error1,wcs,wcslen)
        dy=y1-y2
        dx=x1-x2
        ang_n=90.d0+atan(dy/dx)*rad

        return
        end

