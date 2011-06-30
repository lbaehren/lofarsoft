

        subroutine conv2_get_gaus(sigma,n,m,wid,g)
        implicit none
        include "constants.inc"
        integer n,m,i,j,wid,i1
        real*8 g(n,m),sigma(3),arg1,arg2,ct,st,f1,f2

        do 120 i=1,wid
         do 130 j=1,wid
          i1=(wid+1)/2
          arg1=(i-i1)
          arg2=(j-i1)
          ct=dcos(sigma(3)/rad)
          st=dsin(sigma(3)/rad)
          f1=((arg1*ct+arg2*st)/sigma(1))
          f2=((arg2*ct-arg1*st)/sigma(2))
          g(i,j)=dexp((-0.5d0)*((f1**2.d0)+(f2**2.d0)))
c          g(i,j)=dexp((-1.d0)*((i-i1)**2.d0+(j-i1)**2.d0)/
c     /           (2.d0*sigma*sigma))
130      continue
120     continue

        return
        end


