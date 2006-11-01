      subroutine readimage

C  Read a FITS image and determine the minimum and maximum pixel value.
C  Rather than reading the entire image in
C  at once (which could require a very large array), the image is read
C  in pieces, 100 pixels at a time.  

      integer status,unit,readwrite,blocksize,naxes(2),nfound
      integer group,firstpix,nbuffer,npixels,i
      real datamin,datamax,nullval,buffer(100)
      logical anynull
      character filename*80

C  The STATUS parameter must always be initialized.
      status=0

C  Get an unused Logical Unit Number to use to open the FITS file.
      call ftgiou(unit,status)

C  Open the FITS file previously created by WRITEIMAGE
      filename='ATESTFILEZ.FITS'
      readwrite=0
      call ftopen(unit,filename,readwrite,blocksize,status)

C  Determine the size of the image.
      call ftgknj(unit,'NAXIS',1,2,naxes,nfound,status)

C  Check that it found both NAXIS1 and NAXIS2 keywords.
      if (nfound .ne. 2)then
          print *,'READIMAGE failed to read the NAXISn keywords.'
          return
       end if

C  Initialize variables
      npixels=naxes(1)*naxes(2)
      group=1
      firstpix=1
      nullval=-999
      datamin=1.0E30
      datamax=-1.0E30

      do while (npixels .gt. 0)
C         read up to 100 pixels at a time 
          nbuffer=min(100,npixels)
      
          call ftgpve(unit,group,firstpix,nbuffer,nullval,
     &            buffer,anynull,status)

C         find the min and max values
          do i=1,nbuffer
              datamin=min(datamin,buffer(i))
              datamax=max(datamax,buffer(i))
          end do

C         increment pointers and loop back to read the next group of pixels
          npixels=npixels-nbuffer
          firstpix=firstpix+nbuffer
      end do

      print *
      print *,'Min and max image pixels = ',datamin,datamax

C  The FITS file must always be closed before exiting the program. 
C  Any unit numbers allocated with FTGIOU must be freed with FTFIOU.
      call ftclos(unit, status)
      call ftfiou(unit, status)

C  Check for any error, and if so print out error messages.
C  The PRINTERROR subroutine is listed near the end of this file.
      if (status .gt. 0)call printerror(status)
      end
C *************************************************************************
