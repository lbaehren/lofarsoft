c! to convolve (multiply) a delta with a gauss at a nonintegral pixel posn.
c! bmpix is fw of beam in pix.
c! integral pixel location is centre of pixel. 
c! coordinate referencing ---> round(xposn,yposn) is same as int(n/2,n/2):w
c! this is convolution with constant intensity. need that so source peak fluxes are preserved. 
        
        subroutine get_subim_ptsrc(image,x,n,bmpix,flux,
     /       xposn,yposn,intx,inty,bmvaryn,bmvaryf,beamvary,imsize)
        implicit none
        include "includes/constants.inc"
        integer x,n,i,j,round,intx,inty,imsize,bmvaryn
        real*8 image(x,x),bmpix,flux,xposn,yposn,dumr,xc,yc
        real*8 bmvaryf(bmvaryn),widthx,widthy
        character beamvary

        intx=int(n/2)
        inty=int(n/2)
        xc=xposn-round(xposn)+intx
        yc=yposn-round(yposn)+inty

c! const psf        
        if (beamvary.eq.'n') then
         do i=1,n
          do j=1,n
           dumr=((i-xc)*(i-xc)+(j-yc)*(j-yc))/
     /           (2.d0*bmpix*bmpix/fwsig/fwsig)
           image(i,j)=flux*dexp(-dumr)
          end do
         end do
        end if
c! psf width has sinusoid and a slope, not very physical.
        if (beamvary.eq.'1') then
         widthx=bmvaryf(3)*bmpix+bmvaryf(1)*bmvaryf(3)*
     /          (1.d0+sin(2.d0*pi*bmvaryf(4)*xposn/
     /          imsize))+bmvaryf(2)*bmvaryf(3)*xposn
         widthy=bmpix+bmvaryf(1)*(1.d0+sin(2.d0*pi*bmvaryf(5)*yposn/
     /          imsize))+bmvaryf(2)*yposn
         do i=1,n
          do j=1,n
           dumr=(i-xc)*(i-xc)/(2.d0*widthx*widthx/fwsig/fwsig)+
     /          (j-yc)*(j-yc)/(2.d0*widthy*widthy/fwsig/fwsig)
           image(i,j)=flux*dexp(-dumr)
          end do
         end do
        end if

        return
        end


