      subroutine readheader

C  Print out all the header keywords in all extensions of a FITS file

      integer status,unit,readwrite,blocksize,nkeys,nspace,hdutype,i,j
      character filename*80,record*80

C  The STATUS parameter must always be initialized.
      status=0

C  Get an unused Logical Unit Number to use to open the FITS file.
      call ftgiou(unit,status)

C     name of FITS file 
      filename='ATESTFILEZ.FITS'

C     open the FITS file, with read-only access.  The returned BLOCKSIZE
C     parameter is obsolete and should be ignored. 
      readwrite=0
      call ftopen(unit,filename,readwrite,blocksize,status)

      j = 0
100   continue
      j = j + 1

      print *,'Header listing for HDU', j

C  The FTGHSP subroutine returns the number of existing keywords in the
C  current header data unit (CHDU), not counting the required END keyword,
      call ftghsp(unit,nkeys,nspace,status)

C  Read each 80-character keyword record, and print it out.
      do i = 1, nkeys
          call ftgrec(unit,i,record,status)
          print *,record
      end do

C  Print out an END record, and a blank line to mark the end of the header.
      if (status .eq. 0)then
          print *,'END'
          print *,' '
      end if

C  Try moving to the next extension in the FITS file, if it exists.
C  The FTMRHD subroutine attempts to move to the next HDU, as specified by
C  the second parameter.   This subroutine moves by a relative number of
C  HDUs from the current HDU.  The related FTMAHD routine may be used to
C  move to an absolute HDU number in the FITS file.  If the end-of-file is
C  encountered when trying to move to the specified extension, then a
C  status = 107 is returned.
      call ftmrhd(unit,1,hdutype,status)

      if (status .eq. 0)then
C         success, so jump back and print out keywords in this extension
          go to 100

      else if (status .eq. 107)then
C         hit end of file, so quit
          status=0
      end if

C  The FITS file must always be closed before exiting the program. 
C  Any unit numbers allocated with FTGIOU must be freed with FTFIOU.
      call ftclos(unit, status)
      call ftfiou(unit, status)

C  Check for any error, and if so print out error messages.
C  The PRINTERROR subroutine is listed near the end of this file.
      if (status .gt. 0)call printerror(status)
      end
C *************************************************************************
