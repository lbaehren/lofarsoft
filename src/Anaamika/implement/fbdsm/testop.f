c! to add two images

        implicit none
        integer n,m

        n=5
        m=10
        call sub(n,m)

        return
        end
c!
c!
        subroutine sub(n,m)
        implicit none
        integer n,m,i,j
        character f1*500,extn*20
        real*8 im(n,m)

        do i=1,n
         do j=1,m
          im(i,j)=i*10.d0+j*1.d0
         end do
        end do
        f1='trialop'
        call writearray_bin2D(im,n,m,n,m,f1,'mv')

        return
        end

