c! From NRP. Converted to real*8

      SUBROUTINE fgauss(x,a,y,dyda,na)
      INTEGER na
      REAL*8 x,y,a(na),dyda(na)
      INTEGER i
      REAL*8 arg,ex,fac

        arg=(x-a(2))/a(3)
        ex=dexp((-1.d0)*(arg**2.d0)/2.d0)

        y=a(1)*ex

        dyda(1)=ex
        dyda(2)=y*arg/a(3)
        dyda(3)=y*arg*arg/2.d0 /(3.d0*a(3))

      return
      END
