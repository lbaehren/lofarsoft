      FUNCTION gammp(a,x)
      implicit none

      REAL*8 a,gammp,x
CU    USES gcf,gser
      REAL*8 gammcf,gamser,gln

      if(x.lt.0.d0.or.a.le.0.d0) then
       write (*,*) 'bad arguments in gammp'
       stop 
      end if 
      if(x.lt.a+1.d0)then
        call gser(gamser,a,x,gln)
        gammp=gamser
      else
        call gcf(gammcf,a,x,gln)
        gammp=1.d0-gammcf
      endif
      return
      END
