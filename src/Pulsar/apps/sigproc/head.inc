c==============================================================================
c Definition of the header variables in the time series files.
c==============================================================================
c
      real*8 tsamp   ! sampling time (seconds)
      common /head/ tsamp
      integer nchans,channo,maxchans  ! number of filterbank channels
      parameter(maxchans=4)
      real skyfreq
      common /fbank/ nchans,skyfreq(maxchans),channo
      real refdm     ! reference DM for time series
      common /dmref/ refdm
      real refac     ! reference acceleration for time series (m/s/s)
      real refad     ! reference adot for time series (cm/s/s/s)
      common /accn/ refac,refad
      character*80 filename,header,facc
      integer lst
      common /datafile/ filename,header,lst,facc
      integer dmidx
      common /dmindex/ dmidx
      character*80 zapfile,maskfile(5)
      integer nmasks
      common /zapfname/ zapfile,maskfile,nmasks
      real fbrute
      common /brutal/ fbrute
      integer rfac
      common /rebinfac/ rfac
      real    skp
      common /skipfac/ skp
      integer tsize
      common /transform/ tsize
      logical dumpraw
      common /rawspec/ dumpraw
      logical noise
      common /test/ noise
c
c==============================================================================
