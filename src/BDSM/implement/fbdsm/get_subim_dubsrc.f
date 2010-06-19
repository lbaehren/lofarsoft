
c! to convolve (multiply) a delta with a gauss at a nonintegral pixel posn.
c! bmpix is fw of beam in pix.
c! integral pixel location is centre of pixel. 
c! coordinate referencing ---> round(xposn,yposn) is same as int(n/2,n/2):w
c! this is convolution with constant intensity. need that so source peak fluxes are preserved. 
c! modified to take care of extended sources. if size is larger than beam then 
c! assume its gaussian of beam X size in a random pos.ang.
c!
c! modify. if source size is x, that means put double clean beam gaussian at distance x.
        
        subroutine get_subim_dubsrc(image,x,n,sizepix,beampix,flux,a6,
     /             xposn,yposn,intx,inty,seed,xc1,yc1,xc2,yc2,rand)
        implicit none
        include "includes/constants.inc"
        integer x,n,i,j,round,intx,inty,seed
        real*8 image(x,x),beampix,flux,xposn,yposn,dumr1,dumr2
        real*8 xc1,yc1,xc2,yc2,sizepix,a6,rand,gasdev

        intx=int(n/2)
        inty=int(n/2)
        xc1=intx+sizepix/2*dcos(a6/rad)+xposn-round(xposn)
        yc1=inty+sizepix/2*dsin(a6/rad)+yposn-round(yposn)
        xc2=intx-sizepix/2*dcos(a6/rad)+xposn-round(xposn)
        yc2=inty-sizepix/2*dsin(a6/rad)+yposn-round(yposn)
        do i=1,n
         do j=1,n
          dumr1=((i-xc1)*(i-xc1)+(j-yc1)*(j-yc1))/
     /          (2.d0*beampix*beampix/fwsig/fwsig)
          dumr2=((i-xc2)*(i-xc2)+(j-yc2)*(j-yc2))/
     /          (2.d0*beampix*beampix/fwsig/fwsig)
333       continue
          rand=gasdev(seed)*0.2+0.5  ! is a hack
          if (rand.le.0.1d0.or.rand.ge.0.9d0) goto 333
          image(i,j)=flux*(rand*dexp(-dumr1)+(1.d0-rand)*dexp(-dumr2))
         end do
        end do

        return
        end





