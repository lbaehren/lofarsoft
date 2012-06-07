cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
c     zeropad -- pad leading spaces in a string with zeros
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
      subroutine zeropad(s)
      implicit none
      character*(*) s
      integer i 

      i = 1
      do while (s(i:i) .eq. ' ')
         s(i:i) = '0'
         i = i + 1
      end do
      return
      end
