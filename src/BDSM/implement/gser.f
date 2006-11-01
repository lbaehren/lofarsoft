      SUBROUTINE gser(gamser,a,x,gln)
      INTEGER ITMAX
      REAL*8 a,gamser,gln,x,EPS
      PARAMETER (ITMAX=100,EPS=3.d-7)
CU    USES gammln
      INTEGER n
      REAL*8 ap,del,sumd,gammln
      gln=gammln(a)
      if(x.le.0.d0)then
        if(x.lt.0.d0) then
         write (*,*) 'x < 0 in gser'
         stop
        end if
        gamser=0.d0
        return
      endif
      ap=a
      sumd=1.d0/a
      del=sumd
      do 11 n=1,ITMAX
        ap=ap+1.d0
        del=del*x/ap
        sumd=sumd+del
        if(abs(del).lt.abs(sumd)*EPS)goto 1
11    continue
      write (*,*) 'a too large, ITMAX too small in gser'
      stop
1     gamser=sumd*dexp(-x+a*dlog(x)-gln)

      return
      END
