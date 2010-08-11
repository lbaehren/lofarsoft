
      function erfc(x)
      implicit none

      real*8 erfc,x
CU    USES gammp,gammq
      real*8 gammp,gammq

      if(x.lt.0.d0)then
        erfc=1.d0+gammp(0.5d0,x*x)
      else
        erfc=gammq(0.5d0,x*x)
      endif

      return
      END
