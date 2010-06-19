
      subroutine deletefile(filename,status)
      integer status,unit,blocksize
      character*(*) filename

      if (status .gt. 0)return

      call ftgiou(unit,status)

      call ftopen(unit,filename,1,blocksize,status)

      if (status .eq. 0)then
          call ftdelt(unit,status)
      else if (status .eq. 103)then
          status=0
          call ftcmsg
      else
          status=0
          call ftcmsg
          call ftdelt(unit,status)
      end if

      call ftfiou(unit, status)
      end
