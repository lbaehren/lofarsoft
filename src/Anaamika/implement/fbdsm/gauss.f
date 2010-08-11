
      SUBROUTINE gauss(x,a,y,dyda,na)
      INTEGER na
      REAL*8 x,y,a(na),dyda(na)
      INTEGER i
      REAL*8 arg,ex,fac

      arg=(x-a(2))/a(3)
      ex=dexp(-arg**2.d0)
      fac=a(1)*ex*2.d0*arg
      y=a(1)*ex

      dyda(1)=ex
      dyda(2)=fac/a(3)
      dyda(3)=fac*arg/a(3)

      return
      END

