
        subroutine testfft(image1,x,y,n,m)
        implicit none
        integer x,y,n,m
        real*8 image1(x,y),image(n,m),cimage(n,m)
        double complex speq(m)
        
        call copyarray(image,x,y,n,m,1,1,n,m,cimage)
        call rlft3(image,speq,n,m,1,1)
        call rlft3(image,speq,n,m,1,-1)

        return
        end
