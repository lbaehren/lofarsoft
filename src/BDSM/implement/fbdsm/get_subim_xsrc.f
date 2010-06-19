c! to convolve (multiply) a delta with a gauss at a nonintegral pixel posn.
c! bmpix is fw of beam in pix.
c! integral pixel location is centre of pixel. 
c! coordinate referencing ---> round(xposn,yposn) is same as int(n/2,n/2):w
c! this is convolution with constant intensity. need that so source peak fluxes are preserved. 
c! modified to take care of extended sources. if size is larger than beam then 
c! assume its gaussian of beam X size in a random pos.ang.
        
        subroutine get_subim_xsrc(image,x,n,bmaj,bmin,flux,a6,
     /             xposn,yposn,intx,inty)
        implicit none
        include "includes/constants.inc"
        integer x,n,i,j,round,intx,inty
        real*8 image(x,x),flux,xposn,yposn,dumr,bmin,bmaj
        real*8 a2,a3,a4,a5,a6,arg1,arg2,ct,st,f1,f2

        intx=int(n/2)
        inty=int(n/2)
        a2=xposn-round(xposn)+intx
        a3=yposn-round(yposn)+inty
        a4=bmaj/fwsig
        a5=bmin/fwsig
        
        ct=dcos(a6/rad)
        st=dsin(a6/rad)
        do i=1,n
         do j=1,n
          arg1=(i-a2)
          arg2=(j-a3)
          f1=((arg1*ct+arg2*st)/a4)
          f2=((arg2*ct-arg1*st)/a5)
          dumr=dexp((-0.5d0)*((f1**2.d0)+(f2**2.d0)))
          image(i,j)=flux*dumr
         end do
        end do

        return
        end



