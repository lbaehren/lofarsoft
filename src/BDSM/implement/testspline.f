
        implicit none
        real*8 x(106),y(106),y2(106),xa,ya
        integer i

        open(unit=21,file='op',status='old')
        read (21,*) y
        close(21)
        do i=1,106
         x(i)=i*1.d0
        end do

        call spline(x,y,106,2.d30,2.d30,y2)
        do i=1,105
         xa=i*1.d0+0.2d0
         call splint(x,y,y2,106,xa,ya)
         write (*,*) xa,ya
        end do

        return
        end

