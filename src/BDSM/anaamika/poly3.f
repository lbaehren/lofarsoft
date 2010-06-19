      SUBROUTINE poly3(x,a,y,dyda,na)
      INTEGER na
      REAL*8 x,y,a(na),dyda(na)
      INTEGER i

      y=a(1)+a(2)*x+a(3)*x*x

      dyda(1)=1.d0
      dyda(2)=x
      dyda(3)=x*x

      return
      END

