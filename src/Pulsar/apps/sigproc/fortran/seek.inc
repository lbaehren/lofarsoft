c=============================================================================
c   Common Variable declarations for find (includes common time series/head)
c=============================================================================
c
      include 'vers.inc'
      include 'time.inc'
      include 'head.inc'
      real power   ! power spectrum
      common /four/ power(5,npts/2)
c    
c=============================================================================
    
