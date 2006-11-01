      SUBROUTINE splie2(x1a,x2a,ya,m,n,y2a)
      implicit none  
      INTEGER m,n!,NN
      REAL*8 x1a(m),x2a(n),y2a(m,n),ya(m,n)
c      PARAMETER (NN=100)
CU    USES spline
      INTEGER j,k
c      REAL*8 y2tmp(NN),ytmp(NN)
      REAL*8 y2tmp(n),ytmp(n)

      do 13 j=1,m
        do 11 k=1,n
          ytmp(k)=ya(j,k)
11      continue
        call spline(x2a,ytmp,n,2.d30,2.d30,y2tmp)
        do 12 k=1,n
          y2a(j,k)=y2tmp(k)
12      continue
13    continue
      return
      END
