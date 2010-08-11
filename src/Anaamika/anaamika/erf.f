
      FUNCTION erf(x)
      implicit none
      REAL*8 erf,x
CU    USES gammp
      REAL*8 gammp
      if(x.lt.0.d0)then
        erf=-gammp(0.5d0,x**2.d0)
      else
        erf=gammp(0.5d0,x**2.d0)
      endif

      return
      END
