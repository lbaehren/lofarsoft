      SUBROUTINE splin2(x1a,x2a,ya,y2a,m,n,x1,x2,y)
      implicit none
      INTEGER m,n,NN
      REAL*8 x1,x2,y,x1a(m),x2a(n),y2a(m,n),ya(m,n)
      PARAMETER (NN=100)
CU    USES spline,splint
      INTEGER j,k
      REAL*8 y2tmp(NN),ytmp(NN),yytmp(NN)

      do 12 j=1,m
        do 11 k=1,n
          ytmp(k)=ya(j,k)
          y2tmp(k)=y2a(j,k)
11      continue
        call splint(x2a,ytmp,y2tmp,n,x2,yytmp(j))
12    continue
      call spline(x1a,yytmp,m,2.d30,2.d30,y2tmp)
      call splint(x1a,yytmp,y2tmp,m,x1,y)
      return
      END
