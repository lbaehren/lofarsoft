c! to detect simple discontinuities using 3X3 masks (gonzalez and woods chap 10)

        subroutine detectdiscont(f1,f2)        
        implicit none
        character f1*(*),f2*(*),extn*10
        integer nchar,n,m

        extn='.img'
        call readarraysize(f1,extn,n,m)
        call sub_detectdiscont(f1,f2,n,m)

        return
        end

        subroutine sub_detectdiscont(f1,f2,n,m)
        implicit none
        character f1*(*),f2*(*),extn*10
        integer nchar,n,m,i,j,i1,j1
        real*8 image(n,m),mat(3,3),image1(n,m),dumr,keyvalue

        extn='.img'
        call readarray_bin(n,m,image,n,m,f1,extn)

        call getmatrix(mat)
        
        do i=1,n
         do j=1,m

          dumr=0.d0
          do i1=-1,1
           do j1=-1,1
            if (i+i1.ge.1.and.i+i1.le.n.and.j+j1.ge.1.and.j+j1.le.m)
     /       dumr=dumr+image(i+i1,j+j1)*mat(i1+2,j1+2)
           end do
          end do
          image1(i,j)=dumr

         end do
        end do

        call writearray_bin(image1,n,m,n,m,f2,'mv')

        return
        end

        subroutine getmatrix(mat)
        implicit none
        real*8 mat(3,3),m(3,3)
        integer i,j

        data m/-1.d0,-1.d0,-1.d0,-1.d0,-8.d0,-1.d0,-1.d0,-1.d0,-1.d0/

        do i=1,3
         do j=1,3
          mat(i,j)=m(j,i)
         end do
        end do
        
        return
        end


