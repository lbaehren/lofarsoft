      subroutine writeimage

C  Create a FITS primary array containing a 2-D image

      integer status,unit,blocksize,bitpix,naxis,naxes(2)
      integer i,j,group,fpixel,nelements,array(300,200)
      character filename*80
      logical simple,extend

C  The STATUS parameter must be initialized before using FITSIO.  A
C  positive value of STATUS is returned whenever a serious error occurs.
C  FITSIO uses an `inherited status' convention, which means that if a
C  subroutine is called with a positive input value of STATUS, then the
C  subroutine will exit immediately, preserving the status value. For 
C  simplicity, this program only checks the status value at the end of 
C  the program, but it is usually better practice to check the status 
C  value more frequently.

      status=0

C  Name of the FITS file to be created:
      filename='ATESTFILEZ.FITS'

C  Delete the file if it already exists, so we can then recreate it.
C  The deletefile subroutine is listed at the end of this file.
      call deletefile(filename,status)

C  Get an unused Logical Unit Number to use to open the FITS file.
C  This routine is not required;  programmers can choose any unused
C  unit number to open the file.
      call ftgiou(unit,status)

C  Create the new empty FITS file.  The blocksize parameter is a
C  historical artifact and the value is ignored by FITSIO.
      blocksize=1
      call ftinit(unit,filename,blocksize,status)

C  Initialize parameters about the FITS image.
C  BITPIX = 16 means that the image pixels will consist of 16-bit
C  integers.  The size of the image is given by the NAXES values. 
C  The EXTEND = TRUE parameter indicates that the FITS file
C  may contain extensions following the primary array.
      simple=.true.
      bitpix=16
      naxis=2
      naxes(1)=300
      naxes(2)=200
      extend=.true.

C  Write the required header keywords to the file
      call ftphpr(unit,simple,bitpix,naxis,naxes,0,1,extend,status)

C  Initialize the values in the image with a linear ramp function
      do j=1,naxes(2)
          do i=1,naxes(1)
              array(i,j)=i - 1 +j - 1
          end do
      end do

C  Write the array to the FITS file.
C  The last letter of the subroutine name defines the datatype of the
C  array argument; in this case the 'J' indicates that the array has an
C  integer*4 datatype. ('I' = I*2, 'E' = Real*4, 'D' = Real*8).
C  The 2D array is treated as a single 1-D array with NAXIS1 * NAXIS2
C  total number of pixels.  GROUP is seldom used parameter that should
C  almost always be set = 1.
      group=1
      fpixel=1
      nelements=naxes(1)*naxes(2)
      call ftpprj(unit,group,fpixel,nelements,array,status)

C  Write another optional keyword to the header
C  The keyword record will look like this in the FITS file:
C
C  EXPOSURE=                 1500 / Total Exposure Time
C
      call ftpkyj(unit,'EXPOSURE',1500,'Total Exposure Time',status)

C  The FITS file must always be closed before exiting the program. 
C  Any unit numbers allocated with FTGIOU must be freed with FTFIOU.
      call ftclos(unit, status)
      call ftfiou(unit, status)

C  Check for any errors, and if so print out error messages.
C  The PRINTERROR subroutine is listed near the end of this file.
      if (status .gt. 0)call printerror(status)
      end
C *************************************************************************
