      program tst

      integer NN,MM, i,j
      parameter(NN=4000, MM=4000)

      real*8 data(NN,MM)
      !allocatable data


      !allocate(data(100,100))

      do i = 1,MM
        do j = 1,NN
          data(j,i) = 0
        enddo
      enddo

      data(10,10) = 1.D0

      print *, data(10,10)
      print *, data(11,11)
      call tst_array(data,NN,MM)

      end program


      subroutine tst_array(arr, n, m)
      integer n,m
      real*8 arr(n,m)
      real*8 arr2(n,m)

      !!allocate(arr2(n,m))
      
      print *, n, m, real(n)*m/1024

      do i = 1,m
        do j = 1,n
          arr2(j,i) = 0
        enddo
      enddo

      print *, arr(10,10)
      print *, arr(11,11)

      end
