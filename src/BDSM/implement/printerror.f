
      subroutine printerror(status)
      integer status
      character errtext*30,errmessage*80

      if (status .le. 0)return

      call ftgerr(status,errtext)
      print *,'FITSIO Error Status =',status,': ',errtext

      call ftgmsg(errmessage)
      do while (errmessage .ne. ' ')
          print *,errmessage
          call ftgmsg(errmessage)
      end do
      end
