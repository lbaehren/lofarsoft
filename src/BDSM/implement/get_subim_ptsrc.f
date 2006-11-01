c! to convolve (multiply) a delta with a gauss at a nonintegral pixel posn.
c! bmpix is fw of beam in pix.
c! integral pixel location is centre of pixel. 
c! coordinate referencing ---> round(xposn,yposn) is same as int(n/2,n/2):w
c! this is convolution with constant intensity. need that so source peak fluxes are preserved. 
        
        subroutine get_subim_ptsrc(image,x,n,bmpix,flux,
     /       xposn,yposn,intx,inty,bmvaryn,bmvaryf,beamvary,imsize)
        implicit none
        include "constants.inc"
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
         do i=1,n
          do j=1,n
           widthx=bmpix+bmvaryf(1)*(1.d0+sin(2.d0*pi*bmvaryf(4)*i/
     /            imsize))+bmvaryf(2)*imsize
           widthy=bmpix+bmvaryf(1)*(1.d0+sin(2.d0*pi*bmvaryf(5)*j/
     /            imsize))+bmvaryf(2)*imsize
           dumr=((i-xc)*(i-xc)+(j-yc)*(j-yc))/
     /           (2.d0*widthx*widthy/fwsig/fwsig)
           image(i,j)=flux*dexp(-dumr)
           write (*,*) i,j,bmpix,widthx,n,imsize,
     /            bmvaryf(1)*(1.d0+sin(2.d0*pi*bmvaryf(4)*i/
     /            imsize)),bmvaryf(2)*imsize
          end do
         end do
        end if
        pause

        return
        end


