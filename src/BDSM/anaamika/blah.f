
        subroutine blah(f1)
        implicit none
        character f1*500,extn*20
        integer n,m,l

        extn='.img'
        call readarraysize(f1,extn,n,m,l)
        if (l.gt.1) write (*,*) '  Using 2d array for 3d image !!!'
        call sub_blah(f1,n,m)
        
        return
        end


        subroutine sub_blah(f1,n,m)
        implicit none
        integer n,m,num,i,j
        character f1*500,extn*20
        real*8 image(n,m),val

        extn='.img'
        call readarray_bin(n,m,image,n,m,f1,extn)

        write (*,*) 'number of pixels above what :'
        read (*,*) val

        num=0
        do j=1,m
         do i=1,n
          if (image(i,j).ge.val) num=num+1
         end do
        end do
        write (*,*) num,n*m,num*1.d0/(n*m)
        
        return
        end



