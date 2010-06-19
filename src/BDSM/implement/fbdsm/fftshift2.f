
      subroutine fftshift2(f1,scratch)
      implicit none
      character scratch*500,extn*20,f1*500
      integer nchar,n,m,l

cf2py   intent(in) f1,scratch

      extn='.img'
      call readarraysize(f1,extn,n,m,l)
      call sub_fftshift2(f1,n,m,scratch)

      return
      end

      subroutine sub_fftshift2(f1,n,m,scratch)
      implicit none
      character scratch*500,extn*20,f1*500,f2*500
      integer nchar,n,m,i,j
      real*8 image(n,m),im2(n,m)
     
      extn='.img'
      call readarray_bin(n,m,image,n,m,f1,extn)

      do i=1,int(n/2)
       do j=1,int(m/2)
        im2(i+int(n/2),j+int(m/2))=image(i,j)
       end do
      end do
      do i=int(n/2)+1,n
       do j=1,int(m/2)
        im2(i-int(n/2),j+int(m/2))=image(i,j)
       end do
      end do
      do i=int(n/2)+1,n
       do j=int(m/2)+1,m
        im2(i-int(n/2),j-int(m/2))=image(i,j)
       end do
      end do
      do i=1,int(n/2)
       do j=int(m/2)+1,m
        im2(i+int(n/2),j-int(m/2))=image(i,j)
       end do
      end do

      f2=f1(1:nchar(f1))//'.fs'
      call writearray_bin2D(im2,n,m,n,m,f2,'mv')

      return
      end




