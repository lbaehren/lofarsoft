c! the fdr routine, at last !

        implicit none
        include "constants.inc"
        real*8 x(2000),erfc,y(2000),dumr,dumr1,erf,cons
        integer i

        cons=8.d0/3.d0/pi*(pi-3.d0)/(4.d0-pi)
        do i=1,500
         x(i)=0.01*i
         y(i)=erf(x(i)) 

         dumr1=y(i)
         dumr=sqrt(-2.d0/pi/cons-dlog(1.d0-dumr1*dumr1)/2.d0+
     /    sqrt((2.d0/pi/cons+dlog(1.d0-dumr1*dumr1)/2.d0)*
     /    (2.d0/pi/cons+dlog(1.d0-dumr1*dumr1)/2.d0)-
     /    dlog(1.d0-dumr1*dumr1)/cons))
        write (*,*) x(i),abs((x(i)-dumr)/x(i))
        end do
        
        end
