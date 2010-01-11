c=============================================================================
      subroutine dosearch(llog,dump,rspc,pmzap,sfile,pmax)
c=============================================================================
c
c     Does the search for the pulsars in the Frequency domain.
c
c     - form power spectrum
c     - zaps any interfering bins in spectrum
c     - normalises resulting spectrum to zero mean and unit rms
c     - harmonic summing 1,2,4,8,16 and search for strongest peaks 
c
c=============================================================================
      implicit none
      include 'seek.inc'
      include 'csamp.inc'
      integer llog
      logical dump,rspc,pmzap
      character*80 sfile
c
c     Local variables
c      
      integer h,i,j,k,npf,fold,fb,lun,nc,ncal,istat,n
      real snrbest,rms,sumsq,snrc,thresh,fnyq,spcsnr
      integer indx(npts/8),snum(npts/8)
      integer top,nsm,nf1,c(5),cmin,cmax
      parameter(top=1024,nsm=1024*8)
      real rmea(nsm),rrms(nsm),sres,flo,fhi,fhr,smax
      real*8 freq,pmax,fbest,ratio,pc(top)
      logical harm1,harm3,harm5,harm6
      real*8 pcand(5,top),ptmp
      real scand(5,top),sc(top),spc(512)
      integer nav,ntot,fbin,blo,bhi
      character*80 pfile,ffile

      fold=0

      if (rspc) then
         call readspec(sfile,fold,samp,refdm,refac,tsamp,npf)
         nf1=real(tsamp)*2*npf/real(pmax)
      else
         nf1=real(tsamp)*ntim/real(pmax)
         write(llog,*) 'Forming power spectrum. (Pmax=',pmax,' s!)'
         call formspec(npf,nf1)
         sres=real(freq(tsamp,npf,1,2))-real(freq(tsamp,npf,1,1))
      endif

      fnyq=0.5/real(tsamp)
      write(llog,*) 'Nyquist frequency:',fnyq,' Hz'

      if (dumpraw) then
	  fold=0
	  if (sfile.ne.' ') then
	    pfile=sfile
	  else
            write(pfile,'(a,i1,a)') filename(1:lst)//'_',fold,'.spc'
	  endif
          call writespec(llog,pfile,fold,samp,refdm,refac,tsamp,npf)
	  stop
      else if (dump) then
          write(pfile,'(a4,i1,a4)') 'fold',0,'.spc'
          call writespec(llog,pfile,fold,samp,refdm,refac,tsamp,npf)
      endif 
c
c     Spectral mask
c
      if (maskfile(1).ne.' ') then
        if (nmasks.gt.1) then
           write(llog,*) 'Reading spectral mask...'
        else
           write(llog,*) 'Reading spectral masks...'
        endif
        call glun(lun)
	istat=0
	open(unit=lun,file=maskfile(1),status='old',iostat=istat)
        if (istat.ne.0) then
           write(*,*) 'WARNING - mask file not found...'
        else
           j=0
           do while(istat.eq.0)
              read(lun,*,iostat=istat) i
              if (istat.eq.0) then
                 samp(i)=0.0
                 j=j+1
              endif
           enddo
           write(llog,*) 'Masked',j,' spectral bins from fold 1'
         endif
         close(unit=lun)
      endif
c
c     Zap birdies before doing any spectral manipulation
c        
      if (zapfile.ne.' ') call zapit(llog,1,zapfile,samp,npf,tsamp)

      write(llog,*) 'Whitening spectrum...'

      nav=max(128,npf/nsm)
      write(llog,*) 'Calculating spectral mean/rms every',
     &nav,' bins...',real(freq(tsamp,npf,1,nav+1))
     &-real(freq(tsamp,npf,1,1)),' Hz'
c
c     Form the running median
c      
      call getrmea(samp,npf,nav,rmea,ncal)
c      call getrmed(samp,npf,nav,rmea,ncal)
c
c     Form the rms of the data ater subtracting the mean
c      
      call getrrms(samp,npf,rmea,nav,rrms)
c
c     Subtract running mean and scale it so that the rms=1
c
c      if (fbrute.gt.0.0) 
c     &   write(llog,*) 'Brutal zapping above',fbrute,' Hz!'
      if (fbrute.gt.0.0) 
     &   write(llog,*) 'Brutal zapping below',fbrute,' Hz!'
      n=0
      sumsq=0.0
      do i=1,npf
         h=min(ncal,i/nav+1)
         if (samp(i).ne.0.0) samp(i)=(samp(i)-rmea(h))/rrms(h)
         if (mod(i,1024).eq.0.and.samp(i).lt.3.0) then
            n=n+1
            sumsq=sumsq+samp(i)*samp(i)
         endif
c
c this line is the -b option which brutally zaps all RFI and psrs < fbrute Hz
c changed for 47tuc analysis Mar 17, 2003
	 if (fbrute.gt.0.0.and.freq(tsamp,npf,1,i).lt.fbrute.and.
     &      samp(i).gt.10.0) samp(i)=1.0
      enddo

      rms=sqrt(sumsq/real(n))
      write(llog,*) 'Resulting spectral RMS:',rms
c
c     for Parkes Multibeam Data - call zapping algorithm if -pmzap selected
c
      if (pmzap) call zap_birdies(samp,npf,nf1,tsamp*1000.0,rms,0.0,1)
c
c     original spectrum + 4 harmonic sums (Lyne/Ashworth code)
c      
      write(llog,*) 'Doing harmonic summing...'
      call sumhrm(samp,npf,nf1)
c      
c     mask out folds 2-5 in addition to fold 1 (done above) if 
c     mask files are present (drl - 28/04/05)
c
      if (nmasks.eq.5) then
         do fold=2,5
           j=0
           call glun(lun)
           istat=0
           open(unit=lun,file=maskfile(fold),status='old',iostat=istat)
           do while(istat.eq.0)
              read(lun,*,iostat=istat) i
              if (istat.eq.0) then
                 power(fold,i)=0.0
                 j=j+1
              endif
           enddo
           write(llog,*) 'Masked',j,' spectral bins from fold',fold
           close(lun)
         enddo
      endif

      fb=0
      fbest=0.0
      snrbest=0.0
      ntot=0
c
c     Search for candidates over all harmonic folds
c
      write(llog,*) 'Doing harmonic searching...'
      do fold=1,5
        c(fold)=0
        thresh=5.0
 5      do i=1,npf
          ptmp=1.0/freq(tsamp,npf,fold,i)
          if (ptmp.gt.pmax) power(fold,i)=0.0 ! Zap P>Pmax signals
          snrc=power(fold,i)/rms
          if (snrc.gt.thresh.and.c(fold).lt.top) then
             c(fold)=c(fold)+1
             samp(c(fold))=power(fold,i)
             snum(c(fold))=i
c          else if (snrc.gt.thresh) then
c             write(*,*) 'WARNING: not enough candidates saved!!'
          endif
        enddo
        if (c(fold).lt.2) then
           thresh=thresh-1.0
           goto 5
        endif
c
c       Sort power spectrum in amplitude (propto signal-to-noise) order...
c
        call indexxf77(c(fold),samp,indx)
c
c     
        j=0
        do i=c(fold),1,-1
          j=j+1
          pcand(fold,j)=1000.0/freq(tsamp,npf,fold,snum(indx(i)))
          snrc=samp(indx(i))/rms
          scand(fold,j)=snrc
          if (snrc.gt.snrbest) then
            snrbest=snrc
            fbest=1000.0/pcand(fold,j)
            fb=fold
          endif
        enddo

        do i=1,c(fold)
           do j=1,c(fold)
              if (j.ne.i.and.scand(fold,j).gt.0.0.and.
     &           scand(fold,i).gt.0.0) then
                 ratio=pcand(fold,i)/pcand(fold,j)
                 if (ratio.lt.1.0) ratio=1.0/ratio
                 ratio=ratio-int(ratio)
                 harm1=ratio.gt.0.999.or.ratio.lt.0.001
                 harm3=ratio.gt.0.329.and.ratio.lt.0.331
                 harm5=ratio.gt.0.499.and.ratio.lt.0.501
                 harm6=ratio.gt.0.659.and.ratio.lt.0.661
                 if(harm1.or.harm3.or.harm5.or.harm6)scand(fold,j)=0.0
              endif
           enddo
        enddo

        j=0
        do i=1,c(fold)
           if (scand(fold,i).gt.5.0) then  
              j=j+1
              scand(fold,j)=scand(fold,i)
              pcand(fold,j)=pcand(fold,i)
           endif
        enddo
        c(fold)=j

        do i=1,c(fold)
           ntot=ntot+1
           if (ntot.le.top) then
             sc(ntot)=scand(fold,i)
             pc(ntot)=pcand(fold,i)
           else
c             write(*,*) 'WARNING: master candidate list full!'
           endif
        enddo
        
        if (dump) then
          do i=1,npf
            samp(i)=power(fold,i)
          enddo
          write(pfile,'(a,i1,a)') filename(1:lst)//'_',fold,'.spc'
          write(pfile,'(a,a)') filename(1:lst),'.spc'
          write(pfile,'(a4,i1,a4)') 'fold',fold,'.spc'
          call writespec(llog,pfile,fold,samp,refdm,refac,tsamp,npf)
        endif

        rms=rms*sqrt(2.0) ! Each fold increases rms by root 2...
      enddo

      cmin=top
      do i=1,5
         cmin=min(cmin,c(i))
      enddo
      
      cmax=0
      do i=1,5
         cmax=max(cmax,c(i))
      enddo
      
      call glun(lun)
      pfile=filename(1:lst)//'.prd'
      open(unit=lun,file=pfile,status='unknown',access=facc)
      write(lun,*) 'DM:',refdm,' AC:',refac,' AD:',refad
      do i=1,cmax
         do j=1,5
            if (i.gt.c(j)) then
              pcand(j,i)=1.0
              scand(j,i)=0.0
            endif
         enddo
         write(lun,1) scand(1,i),pcand(1,i),scand(2,i),pcand(2,i),
     &                scand(3,i),pcand(3,i),scand(4,i),pcand(4,i),
     &                scand(5,i),pcand(5,i)
      enddo
      close(unit=lun)
      ffile=filename(1:lst)//'.frq'
      open(unit=lun,file=ffile,status='unknown',access=facc)
      write(lun,*) 'DM:',refdm,' AC:',refac,' AD:',refad
      do i=1,cmax
         write(lun,1) scand(1,i),1000.0/pcand(1,i),scand(2,i),
     &                1000.0/pcand(2,i),scand(3,i),
     &                1000.0/pcand(3,i),scand(4,i),
     &                1000.0/pcand(4,i),scand(5,i),
     &                1000.0/pcand(5,i)
      enddo
      close(unit=lun)

      call indexxf77(ntot,sc,indx)
      do i=ntot,1,-1
         k=indx(i)
         do j=1,ntot
            if (j.ne.k.and.sc(j).gt.0.0.and.
     &           sc(k).gt.0.0) then
               ratio=pc(k)/pc(j)
               if (ratio.lt.1.0) ratio=1.0/ratio
               ratio=ratio-int(ratio)
               harm1=ratio.gt.0.999.or.ratio.lt.0.001
               harm3=ratio.gt.0.329.and.ratio.lt.0.331
               harm5=ratio.gt.0.499.and.ratio.lt.0.501
               harm6=ratio.gt.0.659.and.ratio.lt.0.661
               if(harm1.or.harm3.or.harm5.or.harm6)sc(j)=0.0
            endif
         enddo
      enddo
      
      nc=0
      do i=ntot,1,-1
         if (sc(indx(i)).gt.0.0) then
            do j=1,512
               spc(j)=0.0
            enddo
            smax=0.0
            do h=1,128
               fhr=real(h)*1000.0/pc(indx(i))
               flo=fhr-1000.0/pc(indx(i))*0.05
               blo=fbin(tsamp,npf,1,flo)
               fhi=fhr+1000.0/pc(indx(i))*0.05
               bhi=min(blo+511,fbin(tsamp,npf,1,fhi))
               if (bhi.lt.npf) then
                  j=0
                  do k=blo,bhi
                    j=j+1
                    spc(j)=spc(j)+power(1,k)
                  enddo
                  smax=max(smax,spcsnr(spc,j))
               endif
            enddo
            nc=nc+1
            pcand(1,nc)=pc(indx(i))
            scand(1,nc)=smax
            scand(2,nc)=sc(indx(i))
         endif
      enddo
      do i=1,nc
         sc(i)=scand(2,i) ! scand(1,i) are the "optimised" SNRs (need's work)
      enddo
      call indexxf77(nc,sc,indx)
      
c      ffile=filename(1:lst)//'.opt'
c      open(unit=lun,file=ffile,status='unknown',access=facc)
cc      write(lun,*) 'DM:',refdm,' AC:',refac,' AD:',refad
c      do i=nc,1,-1
c         j=indx(i)
c         ratio=pcand(1,j)/pcand(1,indx(nc))
c         if (ratio.lt.1.0) ratio=1.0/ratio
c         if (sc(j).gt.9.9)write(lun,2) sc(j),pcand(1,j),ratio,scand(2,j)
c      enddo
c      close(unit=lun)
      
 1    format(5(f5.1,1x,f13.8,1x))
 2    format(f5.1,1x,f9.4,3x,f7.3,1x,f5.1)
      
      write(llog,*) 'Best suspect:',1000.0/fbest,' ms'
      write(llog,'(x,a,x,f5.1)') 'SNR:',snrbest
      write(llog,*) 'Found peak at:',fbest,' Hz'
      write(llog,*) 'Harmonic fold:',fb

      ffile=filename(1:lst)//'.top'
      open(unit=lun,file=ffile,status='unknown',access=facc)
      write(lun,*) 1000.0/fbest,snrbest,refdm
      close(unit=lun)

      end
      
