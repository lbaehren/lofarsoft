c=============================================================================
      program seek
c=============================================================================
c
c     A program (formerly known as find) to seek for periodic signals in data
c
c     Created: 97/11/21 (dunc@mpifr-bonn.mpg.de)
c
c     Modification history:
c
c     98/04/10 - (dunc) added acceleration capability (no longer need rbin)
c     98/04/30 - (dunc) source code overhauled. Now more user-friendly
c     98/11/20 - (dunc) added read and FFT capability for ".dis" files
c     99/07/08 - (dunc) added read spectrum file capability
c     01/02/15 - (dunc) added capability to read new data format ".tim" files
c     01/10/11 - (dunc) added -pmzap option and fixed call to zapit routine
c     02/03/01 - (dunc) added -pulse option to call Maura's single-pulse code
c     02/03/20 - (dunc) changed ordering of spectral zapping in dosearch.f
c     02/03/21 - (dunc) added -pzero option to pad with zeros if need be
c     02/03/21 - (dunc) make pmax a command-line parameter
c     05/04/07 - (dunc) changed name of program to SEEK! to appease RNM et al.
c     05/04/28 - (dunc) added ability to read masks for all 5 harmonic folds
c
c=============================================================================
      implicit none
      include 'seek.inc'
      logical dump,rspc,acsearch,tanalyse,pmzap,pulse,append,pzero
      character*80 sfile
      real accn,adot
      real*8 pmax
      integer llog       
      call seekin(llog,dump,rspc,pmzap,sfile,pulse,append,pzero,pmax)
      accn=refac
      adot=refad
      call timstart(llog)                    ! fire up the ship's clock
      if (.not.rspc) call readdat(llog,pzero)! read in the time series 
      if (pulse) then
        call baseline(llog)
        call singlepulse(llog,append)
      endif
c      if (accn.ne.0.0) refac=accn
c      if (adot.ne.0.0) refad=adot
c      acsearch=accn.ne.0.0.or.adot.ne.0.0
c      tanalyse=(index(filename,'.ser').gt.0.0)
c     &     .or.(index(filename,'.tim').gt.0.0)
c     &     .or.(index(filename,'.dis').gt.0.0)
c      if (rspc) tanalyse=.false.
c      if (tanalyse) then                     ! (time series analysis only)
c         if (acsearch) call resample(llog)   ! re-sample time series
c         call fftdata(llog)                  ! fft the data
c      endif                                  ! (standard analysis follows)
c      call dosearch(llog,dump,rspc,pmzap,sfile,pmax)! search Fourier spectrum
      call timfinis(llog)                    ! stop the clock
      end
c=============================================================================
