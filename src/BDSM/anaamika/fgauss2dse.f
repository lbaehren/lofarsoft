c! 2d gaussian, 
c! a4, a5 are like sigma.
c! note that angle is clockwise for rot matrix used. will correct outside.
c! now symmetric gaussian. 

        subroutine fgauss2dse(i,j,a,y,dyda,na)
        real*8 y,a(na),dyda(na)
        real*8 arg1,arg2,ex,fac,ct,st,f1,f2,rad
        integer na,i,j
        rad=180.d0/3.14159d0

        arg1=(i-a(2))/a(4)
        arg2=(j-a(3))/a(4)
        ex=dexp((-0.5d0)*((arg1**2.d0)+(arg2**2.d0)))
        y=a(1)*ex

        dyda(1)=ex
        dyda(2)=y*arg1/a(4)
        dyda(3)=y*arg2/a(4)
        dyda(4)=y*arg1*arg1/a(4)

        return
        end
