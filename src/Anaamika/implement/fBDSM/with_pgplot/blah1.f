
        subroutine blah1(f1,scratch)
        implicit none
        character f1*500,extn*20,scratch*500
        integer n,m,l

        extn='.img'
        call readarraysize(f1,extn,n,m,l)
        if (l.gt.1) write (*,*) '  Using 2d array for 3d image !!!'
        call sub_blah1(f1,n,m)
        
        return
        end


        subroutine sub_blah1(f1,n,m)
        implicit none
        integer n,m,num,i,j
        character f1*500,extn*20
        real*8 image(n,m),val

        extn='.img'
        call readarray_bin(n,m,image,n,m,f1,extn)

333     continue
        write (*,*) ' enter value : '
        read (*,*) val

        num=0
        do j=1,m
         do i=1,n
          if (abs(image(i,j)-val).lt.1.d-5*val) 
     /       write (*,*) i,j,image(i,j)
         end do
        end do
        goto 333
        
        return
        end



