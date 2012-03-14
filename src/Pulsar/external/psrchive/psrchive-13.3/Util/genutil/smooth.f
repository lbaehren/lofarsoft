
      subroutine smooth_mw(pr,nbin,maxwidth, rmsp,kwmax,snrmax,smmax)
        call smooth_mmw(pr,nbin,1,maxwidth,rmsp,kwmax,snrmax,smmax)
      end

      subroutine smooth_mmw(pr,nbin,minwidth,maxwidth, rmsp,kwmax,
     &   snrmax
     &   ,smmax)
c******************************************************************
c
c  convolves profile pr(nbin) with a boxcar of width kw.  it returns
c    the width kwmax which gave the highest s/n ratio snrmax, and the
c    corresponding pulse amplitude smmax.
c
c RTE 17 Feb 00, parameterized maximum boxcar witdth
c
c******************************************************************
c

      integer nbin,kwmax, maxwidth, minwidth
      real*4 pr(*),rmsp,snrmax,smmax
c
      integer ksm,j,k,kw,nn,ja,jj,nvalid
      real*4 s,wrk(2048),al,an,sn,smax
      logical bail
c
c      write(*,*) 'Using new version of smooth_mw'
c      write(*,*) 'At input, nbin is ',nbin,' rmsp is ', rmsp,'maxw ',
c     &  maxwidth

      if (minwidth .lt. 1) then
         minwidth = 1
      endif

c      sanity check
      if (minwidth.gt.maxwidth) then
         write(5,*) 'smooth_mmw error minwidth > maxwidth',
     &      minwidth, maxwidth
         stop
      endif

      snrmax=0.
      if (nbin.gt.1024) stop 'smooth.f Increase dim of wrk in smooth_mw'
c---------------------------------------
c  remove baseline
      ksm=nbin/2.5+0.5
      smax=1.e30
      do 10 j = 1,nbin
        s=0.0
        do 20 k = 1,ksm
          s = s + pr(mod(j+k-1,nbin)+1)
   20   continue
        if(s.lt.smax) smax=s
   10 continue
      smax=smax/ksm
      do 30 j = 1,nbin
        pr(j) = pr(j) - smax
   30 continue
c--------------------------------------
c  count # of valid widths
c
      nvalid = 0
      kw = minwidth
      do 39 nn=1,1000
        if (kw.ge.minwidth .and. kw .le. maxwidth) nvalid = nvalid + 1
        kw=kw*1.5
       if (kw .eq. 1) then
           kw = 2
       endif
 39   continue   
C      write(*,*) 'nvalid is ',nvalid
      if (nvalid .eq. 0) then
         write(5,*) 'smooth_mmw no valid widths', minwidth, maxwidth
         minwidth = nbin/2
         maxwidth = nbin/2
         bail = .true. 
      else
         bail = .false.
      endif
      kw = minwidth
      do 40 nn=1,1000
        if (nvalid .ne. 0 ) then
C          write (*,*) 'kw min max nbin',kw,minwidth,maxwidth,nbin
          if (kw.lt.minwidth) goto 40
          if (kw.gt.maxwidth) goto 70
        else
           kw = nbin/2
           bail = .true.
        endif
C        write(*,*) 'Attempting kw of ',kw
        if(kw.gt.nbin * 0.9) return
	s=0.0
	do 50 k=1,kw
	  s=s+pr(k)
	  wrk(k)=pr(k)
   50   continue
	ja=0
	smax=s
	do 60 j=2,nbin
	  ja=ja+1
	  if(ja.gt.kw) ja=ja-kw
	  al=wrk(ja)
	  jj=j+kw-1
	  if(jj.gt.nbin)jj=jj-nbin
	  an=pr(jj)
	  s=s+an-al
	  wrk(ja)=an
	  if(s.gt.smax) smax=s
   60   continue

        sn=smax/(rmsp*sqrt(kw*(1.+float(kw)/nbin)))
        if(sn.gt.snrmax) then
          snrmax=sn
          kwmax=kw
          smmax=smax/kw
C          write(*,*) ' QQ snrmax now ',snrmax, ' kwmax ', kwmax
        endif
        if (bail) then
           goto 70
        endif
        kw=kw*1.5
        if (kw .eq. 1) then
           kw = 2
        endif
   40 continue

C 70   write(*,*) 'Best width : ',kwmax, ' Best SNR : ', snrmax
 70   continue
      end

C rte old interface
      subroutine smooth(pr,nbin, rmsp,kwmax,snrmax,smmax)
      integer nbin,kwmax, maxwidth
      real*4 pr(*),rmsp,snrmax,smmax
      
      call smooth_mmw(pr,nbin,1, 32, rmsp,kwmax,snrmax,smmax)
      end



