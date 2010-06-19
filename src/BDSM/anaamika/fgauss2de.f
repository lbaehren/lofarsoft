c! 2d gaussian, 
c! a4, a5 are like sigma.
c! note that angle is clockwise for rot matrix used. will correct outside.

        subroutine fgauss2de(i,j,a,y,dyda,na)
        real*8 arg1,arg2,ex,fac,ct,st,f1,f2,rad
        integer na,i,j
        real*8 y,a(na),dyda(na)
        rad=180.d0/3.14159d0

        arg1=(i-a(2))
        arg2=(j-a(3))
        ct=dcos(a(6)/rad)
        st=dsin(a(6)/rad)
        f1=((arg1*ct+arg2*st)/a(4))
        f2=((arg2*ct-arg1*st)/a(5))
        ex=dexp((-0.5d0)*((f1**2.d0)+(f2**2.d0)))
        y=a(1)*ex

        dyda(1)=ex
        dyda(2)=y*(f1/a(4)*ct-f2/a(5)*st)
        dyda(3)=y*(f1/a(4)*st+f2/a(5)*ct)
        dyda(4)=y*f1*f1/a(4)
        dyda(5)=y*f2*f2/a(5)
        dyda(6)=(-1.d0)*y/rad*f1*f2*(a(5)/a(4)-a(4)/a(5))

        return
        end
