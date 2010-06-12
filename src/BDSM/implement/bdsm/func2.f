
      SUBROUTINE func2(x,a,y,dyda,na)
      INTEGER na
      REAL*8 x,y,a(na),dyda(na)
      INTEGER i


       y=a(1)+a(2)*x*a(3)*x*x+a(4)*(x**3.d0)+
     /         a(5)*(x**4.d0)

       dyda(1)=1.d0
       dyda(2)=x
       dyda(3)=x*x
       dyda(4)=x*x*x
       dyda(5)=x*x*x*x


      return
      END

