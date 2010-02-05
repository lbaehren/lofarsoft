      program sumfft
c
c     sums a bunch of .fft files and writes a new one out
c
      implicit none
      include 'npts.inc'
      integer narg,iargc,i,j,ntim
      real tsamp,refdm,refac,series(npts),sum(npts)
      character*80 fftfile

      narg=iargc()
      if (narg.lt.2) then
         write(*,'('' usage: sumfft file1.fft file2.fft ....'')')
         stop
      endif

      do j=1,npts
         sum(j)=0.0
      enddo

      do i=1,narg
         call getarg(i,fftfile)
         write(*,'('' reading '',a)') fftfile(1:index(fftfile,' ')-1)
         open(unit=10,file=fftfile,status='old',form='unformatted')
         read(10) ntim,tsamp,refdm,refac
         read(10) (series(j),j=1,ntim)
         close(unit=10)
         do j=1,ntim
            sum(j)=sum(j)+series(j)
         enddo
      enddo

      write(*,'('' writing sum.fft'')')
      open(unit=10,file='sum.fft',status='unknown',form='unformatted')
      write(10) ntim,tsamp,refdm,refac
      write(10) (sum(j),j=1,ntim)
      close(unit=10)

      end
