c! 2d gaussian, for now y is major axis
c! rotates axes anticlockwise so point is rotated clockwise from +ve x axis

        subroutine fgauss2dne(i,j,a,y,dyda,na)
        integer na,i,j,nmul,isrc
        real*8 y,a(na),dyda(na),arg1,arg2,ex(na/6),why(na/6)
        real*8 fac,ct(na/6),st(na/6),f1(na/6),f2(na/6),rad
        rad=180.d0/3.14159d0

        nmul=na/6  ! number of multiple srcs

        y=0.d0
        do isrc=1,nmul
         arg1=(i-a(2+6*(isrc-1)))
         arg2=(j-a(3+6*(isrc-1)))
         ct(isrc)=dcos(a(6+6*(isrc-1))/rad)
         st(isrc)=dsin(a(6+6*(isrc-1))/rad)
         f1(isrc)=((arg1*ct(isrc)+arg2*st(isrc))/a(4+6*(isrc-1)))
         f2(isrc)=((arg2*ct(isrc)-arg1*st(isrc))/a(5+6*(isrc-1)))
         ex(isrc)=dexp((-0.5d0)*((f1(isrc)**2.d0)+(f2(isrc)**2.d0)))
         why(isrc)=a(1+6*(isrc-1))*ex(isrc)
         y=y+why(isrc)
        end do

        do isrc=1,nmul
         dyda(1+6*(isrc-1))=ex(isrc)
         dyda(2+6*(isrc-1))=why(isrc)*(f1(isrc)/a(4+6*(isrc-1))*ct(isrc)
     /           -f2(isrc)/a(5+6*(isrc-1))*st(isrc))
         dyda(3+6*(isrc-1))=why(isrc)*(f1(isrc)/a(4+6*(isrc-1))*st(isrc)
     /           +f2(isrc)/a(5+6*(isrc-1))*ct(isrc))
         dyda(4+6*(isrc-1))=why(isrc)*f1(isrc)*f1(isrc)/a(4+6*(isrc-1))
         dyda(5+6*(isrc-1))=why(isrc)*f2(isrc)*f2(isrc)/a(5+6*(isrc-1))
         dyda(6+6*(isrc-1))=(-1.d0)*why(isrc)/rad*f1(isrc)*f2(isrc)*
     /           (a(5+6*(isrc-1))/a(4+6*(isrc-1))-a(4+6*(isrc-1))/
     /           a(5+6*(isrc-1)))
        end do

        return
        end
