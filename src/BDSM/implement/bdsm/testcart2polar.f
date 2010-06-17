
        implicit none
        real*8 x(100),y(100),xcen,ycen,rad,pi,r,th
        real*8 theta
        integer i

        pi=3.14159d0
        rad=0.5d0
        xcen=1.d0
        ycen=2.d0
        do i = 1,100
         theta=2.d0*pi/100*i
         x(i)=xcen+rad*dcos(theta)
         y(i)=ycen+rad*dsin(theta)
         call cart2polar(x(i),y(i),xcen,ycen,r,th)
         if (th.lt.0.d0) th=th+2.d0*pi
         write (*,*) x(i),y(i),rad,r,theta*180/pi,th*180/pi
        end do



        return
        end


