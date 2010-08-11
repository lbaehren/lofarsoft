c! put in effect of primary beam

        subroutine do_primarybeam3D(sub_src,ptsizex,ptsizey,nchan,
     /       cdelt1,freq,dia,xc,yc)
        implicit none
        include "constants.inc"
        integer ptsizex,ptsizey,nchan,xc,yc,i,j,k
        real*8 sub_src(ptsizex,ptsizey,nchan),freq,dia
        real*8 cdelt1,sig,dumr,dist2

        sig=c/freq/dia*rad*3600.d0/cdelt1/fwsig   ! pribeam sigma in pixels
        if (xc.eq.0.and.yc.eq.0) then
         xc=ptsizex/2
         yc=ptsizey/2
        end if

        do k=1,nchan
         do i=1,ptsizex
          do j=1,ptsizey
           dist2=(i-xc)*(i-xc)+(j-yc)*(j-yc)
           dumr=dexp((-0.5d0)*(dist2/sig/sig))
           sub_src(i,j,k)=sub_src(i,j,k)*dumr
          end do
         end do
        end do
        
        return
        end


