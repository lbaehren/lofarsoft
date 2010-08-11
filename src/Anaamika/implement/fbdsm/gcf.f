      SUBROUTINE gcf(gammcf,a,x,gln)
      implicit none

      INTEGER ITMAX
      REAL*8 a,gammcf,gln,x,EPS,FPMIN
      PARAMETER (ITMAX=100,EPS=3.d-7,FPMIN=1.d-30)
CU    USES gammln
      INTEGER i
      REAL*8 an,b,c,d,del,h,gammln
      gln=gammln(a)
      b=x+1.d0-a
      c=1.d0/FPMIN
      d=1.d0/b
      h=d
      do 11 i=1,ITMAX
        an=-i*(i-a)
        b=b+2.d0
        d=an*d+b
        if(abs(d).lt.FPMIN)d=FPMIN
        c=b+an/c
        if(abs(c).lt.FPMIN)c=FPMIN
        d=1.d0/d
        del=d*c
        h=h*del
        if(abs(del-1.d0).lt.EPS)goto 1
11    continue
      write (*,*) 'a too large, ITMAX too small in gcf'
      stop  
1     gammcf=dexp(-x+a*dlog(x)-gln)*h

      return
      END
