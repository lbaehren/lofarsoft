c! gets back s1, fac and wid. wid is width of convolving gaussian kernel.
c! fac and s1 are conv constants for calculating convolved intensities so
c! u can get constant intensity or constant flux density convolution.
        
        subroutine conv2_get_wid_fac(sigma,n,m,s1,fac,wid)
        implicit none
        include "constants.inc"
        integer n,m,wid,i,j,i1
        real*8 sigma(3),s1,fac,s2,g,arg1,arg2,ct,st,f1,f2

        wid=min(int(11*max(sigma(1),sigma(2))),min(n,m))
        if (int(wid/2.d0)/(wid/2.d0).eq.1.d0) wid=wid+1
        if (wid.gt.n.or.wid.gt.m) wid=wid-2
        if (wid.lt.7) wid=7

c! gaussian has unit peak. sigma is in pixel width. wid is pixel extent of gaussian.
        s1=0.d0
        s2=0.d0
        do 120 i=1,wid
         do 130 j=1,wid
          i1=(wid+1)/2
          arg1=(i-i1)
          arg2=(j-i1)
          ct=dcos(sigma(3)/rad)
          st=dsin(sigma(3)/rad)
          f1=((arg1*ct+arg2*st)/sigma(1))
          f2=((arg2*ct-arg1*st)/sigma(2))
          g=dexp((-0.5d0)*((f1**2.d0)+(f2**2.d0)))
c          g=dexp((-1.d0)*((i-i1)**2.d0+(j-i1)**2.d0)/
c     /           (2.d0*sigma*sigma))
          s1=s1+g
          s2=s2+g*g
130      continue
120     continue
        fac=sqrt(s2/(s1*s1))
        
        return
        end


