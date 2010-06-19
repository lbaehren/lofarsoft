
        subroutine func3(x,a,y,dyda,na)
        implicit none
        integer na,i
        real*8 x,y,a(na),dyda(na)

        y=sqrt(a(1)*a(1)+a(2)*a(2)/(x*x)) 

        dyda(1)=a(1)/y
        dyda(2)=a(2)/(y*x*x)

        return
        end

