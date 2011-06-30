c! 2d gaussian, for now y is major axis

      subroutine fgauss2d(i,j,a,y,dyda,na)
      real*8 y,a(na),dyda(na)
      real*8 arg1,arg2,ex,fac
      integer na,i,j

      arg1=(i-a(2))/a(4)
      arg2=(j-a(3))/a(5)
      ex=dexp((-0.5d0)*((arg1**2.d0)+(arg2**2.d0)))
      y=a(1)*ex

      dyda(1)=ex
      dyda(2)=y*arg1/a(4)
      dyda(3)=y*arg2/a(5)
      dyda(4)=y*arg1*arg1/a(4)
      dyda(5)=y*arg2*arg2/a(5)

      return
      end
