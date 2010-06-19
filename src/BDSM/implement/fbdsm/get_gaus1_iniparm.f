c! get initial gaussian guess parameters assuming one single peak in image.

        subroutine get_gaus1_iniparm(image,x,y,n,m,a)
        implicit none
        integer x,y,n,m,xmax,ymax
        real*8 image(x,y),a(6),m1,m2(2),m3(2),dum
        real*8 maxv

        call maxarray(image,x,y,n,m,xmax,ymax,maxv)
        a(1)=maxv
        a(2)=xmax
        a(3)=ymax
        
        call moment(image,x,y,n,m,m1,m2,m3)
        a(4)=m3(1)
        a(5)=m3(2)
        a(6)=0.d0
        if (a(4).lt.a(5)) then
         dum=a(4)
         a(4)=a(5)
         a(5)=dum
        end if
        if (a(4)/a(5).le.1.05) a(5)=a(4)*0.7d0

        return
        end


