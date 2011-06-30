
      implicit none
      include "trhermcoef.inc"
      integer n,m,nmax,nx,ny
      parameter (n=32)
      parameter (m=32)
      real*8 beta,hcx(50),hcy(50),B(n,m),xc,yc
      character runcode*2,f1*500

      write (*,*) 'order n m = '
      read (*,*) nx,ny
      nmax=10
      beta=3.d0

      call getcoeffarr(hc,31,31,nx,hcx) 
      call getcoeffarr(hc,31,31,ny,hcy) 

      xc=n/2.d0
      yc=m/2.d0
      call getcartim(n,m,beta,nx,ny,hcx,hcy,xc,yc,nmax,B)
      f1='try'
      runcode='mv'
      call writearray_bin2d(B,n,m,n,m,f1,runcode)

      end
