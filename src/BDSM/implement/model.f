c to compute various things for lofar confusion and sad and so on

        implicit none
        integer nbin,i,dumi,j,npix_src(3),n_src,i1,i2
        integer indexlim(5),sizecut(5)
        integer nbin1,idc,dcmin,dcmax,nc,chan
        real*8 flux(1000),nsrc(1000),naxis1,cdelt1,beam,lev(31)
        real*8 ss(1000),nss(1000),dumr,asec2rad,cutoff,area
        real*8 levval(31),actnoise,num_beam,bmpersrc,erf
        real*8 fpix_false(3),nsrc1(1000),ndiff(1000)
        real*8 ssdiff(1000),nssdiff(1000),h,x(1000),y(1000),pi
        real*8 avgam,omega_e(5),freq(5),lam(5),pb(5),c,sigma(1000,5)
        real*8 xfit(500),yfit(500),sig(500),a,b,siga,sigb,chi2,q
        real*8 k0,que(1000,5),alpha,fluxf(1000,5),ifreq,lofarbl
        real*8 sig5,dumr1,bwtot,flux0(300,5),flux1(300,5),rms1,rms2
        real*8 n1,n2,ealp,alp(8),qmini,fmax(5)
        integer minq,maxq,qcut(5)
        real*8 synbeam(5),slimbeam(5),limit(5),R(1000,5)
        real*8 synbeama(5),prob,k,nsdc(5),ssdc(5),sigmasdc(5)
        real*8 qsdc(5),sens(5),mini(2),maxi(2),differ(300)
        character filen*8,str1*50,str2*4,dir*500
        real*4 mnx,mxx,mny,mxy,x4(1000),y4(1000),x41(10),y41(10)
        real*4 x42(10),y42(10)
        real*4 sizeas(200),hpsi(200)
	real*8 tsys(5),kjy(5),effarea(5),tsky(5),ssky(5),sfluxsn(5)
	real*8 fluxsnL(5),numnick(5)

c! sqrt(levs) 4 times are the levels of distances in pixel units for grid 4X4
        data lev/0.0,1.0,2.0,4.0,5.0,5.0,8.0,9.0,10.0,10.0,13.0,13.0,
     /       16.0,18.0,17.0,17.0,20.0,20.0,25.0,25.0,32.0,25.0,26.0,
     /       26.0,29.0,29.0,34.0,34.0,41.0,41.0,50.0/
        data freq/30.d0,75.d0,120.d0,200.d0,1400.d0/    ! Mhz
        data sens/2.d-3,1.3d-3,0.07d-3,0.06d-3,0.d0/    ! rms Jy single pol 1 hr 4 MHz
	data tsys/23000.d0,2450.d0,820.d0,270.d0,0.d0/
	data effarea/1.9d5,3.0d4,1.9d5,6.9d4,0.d0/ 

        c=3.d8
        pi=3.14159d0
        k=1.38d-23
        asec2rad=pi/(180.d0*3600.d0)
        lofarbl=100000.d0   ! m

        write (*,*)
        write (*,*) ' Freq    Pri_beam   syn_beam   Tsys'
        do 180 i=1,4
         lam(i)=c/(freq(i)*1.d6)
         pb(i)=lam(i)/50.d0 ! primary beam in radian for station size 50 m
         synbeam(i)=1.22d0*lam(i)/lofarbl ! synthesized beam in radian
         synbeama(i)=pi*0.25d0*(synbeam(i)*synbeam(i))  !  beam area in str
         write (*,*) freq(i),pb(i)*180.d0/pi,lam(i)/lofarbl/asec2rad
     /         ,sens(i)*1.d-26*effarea(i)/(2.d0*k)*sqrt(4e6*3600.d0)
180     continue
        write (*,*) '  # of pixels/side (4pix/beam) ',4.0*lofarbl/50.d0

        write (*,*)
        filen='data'
        dir="./"
        call getline(filen,dir,nbin)
        open(unit=21,file=filen,status='old')
         do 100 i=1,nbin
          read (21,*) ss(i),nss(i)       !  mJy and N(>S)/str
          ss(i)=ss(i)*1.0d-3             !   Jy
100      continue
        close(21)

c! i know s is equal intervaled in log or in delS/S. but locally its not that diff so 
c! go ahead and just be careful while integrating that bins arent equally spaced.
        do 110 i=1,nbin-1
         flux(i)=sqrt(ss(i)*ss(i+1))
         ndiff(i)=nss(i)-nss(i+1)
110     continue
        dumr=10.0
        do 120 i=nbin,nbin+int(dumr)-1
         flux(i)=flux(i-1)+flux(i-1)-flux(i-2)
         ndiff(i)=nss(nbin)/dumr
120     continue
        nbin1=nbin+int(dumr)-1

        open(unit=21,file='inputpara',status='old')
        read (21,*) naxis1   ! number of pixels in map
        read (21,*) cdelt1   ! arcsec/pixel
        read (21,*) beam     ! sqrt(bmaj*bmin) in arcsec
        read (21,*) actnoise ! average rms in image 
        close(21)

c calculate nsrc/str * area of image, into an integer
        area=((naxis1*cdelt1*asec2rad)**2.0)           ! str
        do 140 i=1,nbin1
         nsrc(i)=ndiff(i)*area
140     continue
        call intnumct(nbin1,nsrc,nsrc1)

c! calculate total number of sources greater than n sigma and # of beams
        cutoff=3.0
        num_beam=(naxis1*cdelt1/beam)**2.0
        call calcsrcnsigma(cutoff,nbin1,flux,actnoise,nsrc1,num_beam,
     /       n_src,bmpersrc)

c! assume each source is at centre of a pixel which isnt true
c! for now assume no source overlaps which is even less true
        cutoff=3.d0 ! cutoff in sigma below which pixel is background
        call srcpix(cutoff,lev,cdelt1,beam,levval,flux,actnoise,
     /       nsrc1,npix_src,nbin1)

c! number of background pixels above cutoff
        call falspix(fpix_false,cutoff,levval)
        write (*,*) npix_src(1),naxis1,fpix_false(1)

        write (*,'(a16,f4.1,a6)') ' Cut assumed is ',cutoff,'-sigma'
        write (*,'(a34,i5)') ' Number of sources above cutoff = ',n_src
        write (*,'(a30,f8.1)') ' Number of beams per source = ',bmpersrc
        write (*,777) ' Source pixels (peak > cut)  = ',
     /                  npix_src(1)/(naxis1*naxis1)*100,' %'
        write (*,777) ' False pixel (1 pix) rate    = ',fpix_false(1)
     /                *100,' %'
        write (*,*)
        write (*,777) ' Source pixels (5pix > cut)  = ',
     /                  npix_src(2)/(naxis1*naxis1)*100,' %'
        write (*,777) ' False pixel (5 pix) rate    = ',fpix_false(2)
     /                *100,' %'
        write (*,*)
        write (*,777) ' Source pixels (9pix > cut)  = ',
     /                  npix_src(3)/(naxis1*naxis1)*100,' %'
        write (*,777) ' False pixel (9 pix) rate    = ',fpix_false(3)
     /                *100,' %'
        write (*,*)
        write (*,*)
777     format(a31,f7.5,a2)


c!
c! Now to calculate condon's eqn 14 and so on.
c!
c! nsrc(i) is diff number counts upto nbin. 
c! THIS IS WRONG -- do later. assume omega_e has a gamma but there is no
c! single gamma for our #count. take average gamma as 2.18 for now
        do 730 i=1,nbin
         xfit(i)=i*1.d0
         yfit(i)=dlog10(nsrc(i))
         sig(i)=yfit(i)*1.d-5
730     continue
        call fit(xfit,yfit,500,sig,0,a,b,siga,sigb,chi2,q)
        avgam=-b*(xfit(nbin)-xfit(1))/
     /        (dlog10(flux(nbin))-dlog10(flux(1)))
        write (*,*)
        write (*,*) ' Average gamma = ',avgam
        write (*,*)

        do 700 i=1,5
         omega_e(i)=pi/4.d0*pb(i)*pb(i)/(dlog(2.d0)*(avgam-1.d0))
         omega_e(i)=pi/4.d0*synbeam(i)*synbeam(i)/
     /              (dlog(2.d0)*(avgam-1.d0))
700     continue

c! calc fluxes for other freq assuming nu^-alpha        
        alpha=0.8d0   !  S propto nu^-alpha
        do 740 ifreq=1,5
         do 750 j=1,nbin
          fluxf(j,ifreq)=flux(j)*((freq(ifreq)/freq(5))**(-alpha))
750      continue
740     continue

        k0=5.d-4
        do 900 i=1,nbin
         do 910 j=1,5
          R(i,j)=omega_e(j)*ndiff(i)
910      continue
900     continue

c! calculate based on condon 74 eqn 14
        dcmin=10
        dcmax=nbin-1
        do 760 ifreq=1,5
         do 710 idc=dcmin,dcmax
          sigma(idc,ifreq)=0.d0
          do 720 i=1,nbin-1
           if (fluxf(i,ifreq).le.fluxf(idc,ifreq)) then
            if (nsrc(i).gt.0.5) then
             sigma(idc,ifreq)=sigma(idc,ifreq)+
     /         omega_e(ifreq)*fluxf(i,ifreq)
     /         *fluxf(i,ifreq)*nsrc(i)*(fluxf(i+1,ifreq)-fluxf(i,ifreq))
            end if
           end if
720       continue
          sigma(idc,ifreq)=dsqrt(sigma(idc,ifreq))
          que(idc,ifreq)=fluxf(idc,ifreq)/sigma(idc,ifreq)
710      continue
760     continue
c!
c! now calculate the value of sigma and slim for q=3 or min q
c!
	qmini=5.d0
        do 721 i=1,4
	 minq=dcmin
	 maxq=dcmin
	 do 722 j=dcmin,dcmax
          if (que(j,i).lt.que(minq,i)) minq=j
          if (que(j,i).gt.que(minq,i)) maxq=j
722      continue
         if (que(minq,i).gt.qmini) then
	  qcut(i)=minq
	 else
          if (que(maxq,i).lt.qmini) then
	   qcut(i)=9999   ! problem !
	  else    ! qmini is there
	   do 723 j=dcmax,dcmin,-1   ! cos nonmonotonic and i want first cut at q=3
	    if (que(j,i).ge.qmini) then
	     qcut(i)=j
	    end if
723        continue
	  end if
	 end if
c         write (*,778) 'At ',freq(i),' MHz, q at ',qcut(i),' is ',
         write (*,*) 'At ',freq(i),' MHz, q at ',qcut(i),' is ',
     /      que(qcut(i),i),'; sig= ', sigma(qcut(i),i)*1.d3,
     /      ' mJy ; slim = ',fluxf(qcut(i),i)*1.d3,' mJy'
721     continue
778     format(a3,f4.0,a11,i4,a4,f5.2,a8,f10.5,a13,f10.5,a4)
c!
c!  Now calculate which flux do u get 1 src/beam and then get dynamic range from that.
c!  Replace this with proper equivalent of Gumbel distribution fn when u understand
c!  what it is. 
c!
	 write (*,*) '  Brightest source in beam is '
         do 400 i=1,4
          do 405 j=1,nbin
           if (R(j,i).gt.1.d0) fmax(i)=fluxf(j,i) ! Jy
405       continue
          if (R(nbin,i).gt.1.d0) fmax(i)=fluxf(nbin,i)
          write (*,781) freq(i),fmax(i)
400      continue
781      format('For ',f4.0,' MHz, ',f8.2,' Jy')

c! now calculate s_limit assuming limit for conf is 1 src/20beam which is silly
        write (*,*) 
        write (*,*) ' 10 beams per src criterion is '
        write (*,*) '     For LOFAR100'
        do 770 i=1,4
         limit(i)=1.d0/(10.d0*synbeama(i))     ! in 1/str
         do 780 j=1,nbin
          if (nss(j).gt.limit(i)) slimbeam(i)=fluxf(j,i)
780      continue
         write (*,*) freq(i),slimbeam(i)*1.d3,' mJy; ',
     /       (sens(i)/(slimbeam(i)/3.d0))**2.d0,' hrs'
770     continue
        write (*,*) '     For LOFAR VC'
        do 771 i=1,4
         limit(i)=1.d0/(10.d0*synbeama(i)*lofarbl*lofarbl/(2.d3*2.d3))     ! in 1/str
         do 782 j=1,nbin
          if (nss(j).gt.limit(i)) slimbeam(i)=fluxf(j,i)
782      continue
         write (*,*) freq(i),slimbeam(i),' Jy; ',
     /     (sens(i)/(slimbeam(i)/3.d0))**2.d0*60.d0,' mins'
771     continue


c! now calculate in terms of P and k from Vaisanen et al

	write (*,*) 
	write (*,*) '  Sigma (source density criterion'
        prob=0.06       !  1 % that a src > slim is within k*synbeam away
        k=1.d0
        do 370 j=1,4
         nsdc(j)=-dlog10(1.d0-prob)/(pi*k*k*synbeam(j)*synbeam(j))
         do 375 i=1,nbin
          if (nss(i).gt.nsdc(j)) ssdc(j)=fluxf(i,j)
375      continue
         sigmasdc(j)=0.d0
         do 377 i=1,nbin-1
          if (fluxf(i,j).le.ssdc(j)) then
           if (nsrc(i).gt.0.5) then
            sigmasdc(j)=sigmasdc(j)+omega_e(j)*fluxf(i,j)
     /          *fluxf(i,j)*nsrc(i)*(fluxf(i+1,j)-fluxf(i,j))
           end if
          end if
377      continue
         sigmasdc(j)=sqrt(sigmasdc(j))
         qsdc(j)=ssdc(j)/sigmasdc(j)
	 write (*,*) freq(j),' MHz.sig= ',sigmasdc(j)*1.d3,' mJy',
     /         ssdc(j)
370     continue
	write (*,*) 

c!
c! Now calculate min flux for detecting spectral index across 32 MHz BW
c! at 80, 120 and 200 MHz. assume 10 hrs integ, 5 sigma. 16 channels/32 MHz
c! s \propto \nu^-alpha. error in spin is 0.1
c! If we calculate the needed flux at freq nu then its independent of alpha.
c! hence calc for alp=0.8 and as fn of int time
c!
       write (*,*) 
       write (*,*) ' For delta_alpha=0.1, 32 MHz BW and 8 channels'
        ealp=0.1d0   ! error in alpha
        bwtot=32.d0   ! MHz
        chan=8       ! number of channels
        alpha=0.8d0  ! as fiducial value
        do 295 i=2,4              ! freq
         n1=freq(i)-bwtot/2.d0+bwtot/chan*0.5d0
         n2=freq(i)+bwtot/2.d0-bwtot/chan*0.5d0
         dumr1=sqrt(((1400.d0/n1)**(-2.d0*alpha))+
     /               ((1400.d0/n2)**(-2.d0*alpha)))
         do 300 j=1,100             ! integ time in hrs
          differ(j)=sens(i)/sqrt(j*1.d0*(bwtot/chan)/4.d0)  ! 1 chan, T hrs in Jy
          dumr=differ(j)/(ealp*dlog(n1/n2))
          flux0(j,i)=abs(dumr1*dumr)   ! at 1400 MHz
          flux0(j,i)=flux0(j,i)*((freq(i)/1400.d0)**(-alpha))
300      continue   ! spin
         write (*,'(2x,a7,i3,a8,i3)') 'SNR at ',int(freq(i)),' MHz is ',
     /         int(flux0(20,i)/differ(20))
295     continue  ! freq
        write (*,*) 
c!
c! now for same between 120 and 200 MHz
c!
        ealp=0.1d0   ! error in alpha
        bwtot=32.d0   ! MHz
        chan=1
        alpha=0.4d0  ! as fiducial value
        n1=freq(3)
        n2=freq(4)
        do 320 j=1,100             ! integ time in hrs
         rms1=sens(3)/sqrt(j*1.d0*(bwtot/chan)/4.d0)
         rms2=sens(4)/sqrt(j*1.d0*(bwtot/chan)/4.d0)
         do 325 i=1,4
          alp(i)=0.4d0+i*0.2d0
          dumr1=sqrt(rms1*rms1*((1400.d0/n1)**(-2.d0*alp(i)))+
     /              rms2*rms2*((1400.d0/n2)**(-2.d0*alp(i))))
          dumr=1.d0/(ealp*dlog(n1/n2))
          flux1(j,i)=abs(dumr1*dumr)   ! at 1400 MHz
          flux1(j,i)=flux1(j,i)*((freq(i)/1400.d0)**(-alp(i)))
325      continue
320     continue   ! spin

c!
c! now to calculate number of srcs at each freq which is in source noise regime
c!
	write (*,*)
	write (*,*) ' For source noise '
	write (*,*) ' freq   flux      #/str    #/beam '
        do 340 i=1,4
         area=2.d0*pi*0.25d0/log(2.0)*pb(i)*pb(i)      ! 2FWHM primary beam in str
         tsky(i)=400.d0*((freq(i)/150.d0)**(-2.9d0))   ! = tsys in K
	 kjy(i)=effarea(i)/(2.d0*1.38d-23)*1.d-26      ! K/Jy
	 ssky(i)=tsky(i)/kjy(i)   ! Tsky in Jy 
	 sfluxsn(i)=ssky(i)/2.d0  ! src flux in Jy for src noise to be important
	 fluxsnL(i)=sfluxsn(i)*((1400.d0/freq(i))**(-0.8d0))  ! L band flux for same
         call nicknum(fluxsnL(i),numnick(i))  ! numnick is # src (>fluxsnL) per str
	 write (*,787) freq(i),fluxsnL(i),numnick(i),numnick(i)*area
340     continue
787     format(2x,f4.0,3x,f5.2,3x,f7.2,3x,f5.2)



c! plot q vs slim for each freq and also 1/20beam thingy
cc        call pgbegin(0,'pgplot1.ps/VPS',1,2)
c        call pgbegin(0,'pgplot1.ps/VPS',2,2)
c        call pgslw(3)

        call range3(fluxf,1000,5,nbin,4,mnx,mxx,1,0)      ! 1st fig in pgplot.ps
        call range3(que,1000,5,nbin,4,mny,mxy,1,1)
c        call pgpanl(1,1)
c        call pgvport(0.15,0.9,0.15,0.9)
c        call pgsch(1.5)
        mnx=mnx+3.0
        mxx=mxx+3.0
c        call pgwindow(mnx,mxx,mny,mxy)
c        call pgbox('BCNLST',0.0,0,'BCNLST',0.0,0)
c        call pglabel('S\\d limit \\u (mJy)','q',' ')
        do 800 i=1,4
         do 810 j=dcmin,dcmax
          x4(j-dcmin+1)=dlog10(fluxf(j,i)*1.d3)
          y4(j-dcmin+1)=dlog10(que(j,i))
          if (fluxf(j,i).eq.slimbeam(i)) then
c           call pgsch(2.5)
c           call pgpoint(1,x4(j-dcmin+1),y4(j-dcmin+1),16)
c           call pgsch(1.5)
          end if
810      continue
c         call pgsls(i)
c         call pgline(dcmax-dcmin+1,x4,y4)
         x41(i)=dlog10(ssdc(i)*1.d3)
         y41(i)=dlog10(qsdc(i))
800     continue
c        call pgsch(2.5)
cc!        call pgpoint(4,x41,y41,8)
c        call pgsch(1.5)
        call plotfundae(mnx,mxx,mny,mxy,4)

	write (*,*) ' Conf NOISE for 20 bm/src criterion '
        call range3(fluxf,1000,5,nbin,4,mnx,mxx,1,0)      ! 2nd fig in pgplot.ps
        call range3(sigma,1000,5,nbin,4,mny,mxy,1,1)
c        call pgpanl(1,2)
c        call pgvport(0.15,0.9,0.15,0.9)
c        call pgsch(1.5)
        mny=mny+3.0
        mxy=mxy+3.0
        mnx=mnx+3.0
        mxx=mxx+3.0
c        call pgwindow(mnx,mxx,mny,mxy)
c        call pgbox('BCNLST',0.0,0,'BCNLST',0.0,0)
c        call pglabel('S\\d limit \\u (mJy)','\\gs (mJy)',' ')
c        call pgsch(1.0)
        do 820 i=1,4
         do 830 j=dcmin,dcmax
          x4(j-dcmin+1)=dlog10(fluxf(j,i)*1.d3)
          y4(j-dcmin+1)=dlog10(sigma(j,i)*1.d3)
          if (fluxf(j,i).eq.slimbeam(i)) then
c           call pgsch(2.5)
c           call pgpoint(1,x4(j-dcmin+1),y4(j-dcmin+1),16)
	   write (*,*) freq(i),sigma(j,i)*1.d6,' uJy',fluxf(j,i)/5.d-6
c           call pgsch(1.0)
          end if
830      continue
c         call pgsls(i)
c         call pgline(dcmax-dcmin+1,x4,y4)
         x41(i)=dlog10(ssdc(i)*1.d3)
         y41(i)=dlog10(sigmasdc(i)*1.d3)
820     continue
c        call pgsch(2.5)
cc!        call pgpoint(4,x41,y41,8)
c        call pgsch(1.0)
c!        call plotfundae(mnx,mxx,mny,mxy,4)

c        call pgpanl(2,1)                                  ! 3rd fig in pgplot.ps
c        call pgsls(1)
c        call pgvport(0.15,0.9,0.15,0.9)
        call range3(fluxf,1000,5,nbin,4,mnx,mxx,1,0)
        call range3(R,1000,5,nbin,4,mny,mxy,1,1)
c        call pgsch(1.5)
        mny=-1.0
        mnx=mnx+3.0
        mxx=mxx+3.0
c        call pgwindow(mnx,mxx,mny,mxy)
c        call pgbox('BCNLST',0.0,0,'BCNLST',0.0,0)
c        call pglabel('S\\d limit \\u (mJy)','R',' ')
c        call pgsch(1.0)
        do 850 i=1,4
         do 860 j=1,nbin
         x4(j)=dlog10(fluxf(j,i)*1.d3)
          y4(j)=dlog10(R(j,i))
860      continue
c          call pgsls(5-i)
c          call pgline(nbin,x4,y4)
850     continue
        call plotfundae(mnx,mxx,mny,mxy,4)

c        call pgpanl(2,2)                                 ! 4th fig in pgplot.ps
c        call pgvport(0.15,0.9,0.15,0.9)
c        call pgsch(1.5)
        call range3(fluxf,1000,5,nbin,4,mnx,mxx,1,0)
        mny=-1.0
        mxy=2.5
        mnx=mnx+3.0
        mxx=mxx+3.0
        mxx=2.0
c        call pgwindow(mnx,mxx,mny,mxy)
c        call pgbox('BCNLST',0.0,0,'BCNLST',0.0,0)
c        call pglabel('S\\d limit \\u (mJy)','Num beams/src',' ')
c        call pgsch(1.0)
        do 880 i=1,4
         do 890 j=1,nbin
         x4(j)=dlog10(fluxf(j,i)*1.d3)
         y4(j)=dlog10(1.d0/(nss(j)*synbeama(i)))
890      continue
c         call pgsls(5-i)
c         call pgline(nbin,x4,y4)
880     continue
        call plotfundae(mnx,mxx,mny,mxy,4)

c        call pgend
c!
c!
c!
c        call pgbegin(0,'pgplot2.ps/VPS',1,1)
c        call pgslw(3)
c        call pgsch(1.5)
c        call pgvport(0.15,0.9,0.15,0.9)
c        call pgwindow(-0.2,2.1,0.8,3.5)
c        call pgbox('BCLNST',0.0,0,'BCLNST',0.0,0)
c        call pglabel('Integration time (hrs)',
 
c        call pgtext(0.2,3.6,'For LOFAR; Bandwidth=4 MHz')
	alpha=0.8d0
        do 230 j=1,4                     ! freq
         do 220 i=1,100
          sig5=5.0*sens(j)/sqrt(i*1.0)   ! 4 MHz BW
          x4(i)=log10(i*1.0)            ! integ time in hrs
          do 240 i1=1,nbin  
           dumr=ss(i1)*((freq(j)/1400.d0)**(-alpha))
           if (sig5.gt.dumr) y4(i)=nss(i1)
240       continue
          y4(i)=log10(1.0/(y4(i)*synbeama(j)))
	  if (y4(i).gt.log10(9.05).and.y4(i).lt.log10(10.05)) 
     /    write (*,*) freq(j),(1.d0/(10.d0**y4(i)))/synbeama(j),i
     /       ,sig5
220      continue
c         call pgsls(j)
c         call pgline(100,x4,y4)
230     continue
        do 313 i=1,4
c         call pgsls(i)
c         call pgmove(-0.1,1.4-(i-1)*0.15)
c         call pgdraw(0.2,1.4-(i-1)*0.15)
313     continue
c        call pgsch(1.0)
c        call pgtext(0.3,1.40,' 30 MHz')
c        call pgtext(0.3,1.25,' 75 MHz')
c        call pgtext(0.3,1.10,'120 MHz')
c        call pgtext(0.3,0.95,'200 MHz')
c        call pgsls(1)

c        call pgend
c!
c!
c!
c        call pgbegin(0,'pgplot3.ps/VPS',1,1)
c        call pgwindow(2.5,6.5,-1.0,6.0)
c        call pgbox('BCNST',0.0,0,'BCNLST',0.0,0)
c        call pglabel('Sigma','Source pixels/False pixels number',' ')
c        call pgtext(2.6,6.2,
 
        call plotfundae(2.5,6.5,-1.0,6.0,4)

        naxis1=16000
        do 920 i=1,4      ! index for freq
         beam=synbeam(i)/asec2rad
         cdelt1=beam/4.d0
         area=((naxis1*cdelt1*asec2rad)**2.0)           ! str
         do 925 j=1,nbin1
          nsrc(i)=ndiff(i)*area
925      continue
         actnoise=sens(i)
         do 930 j=1,7     ! 5 index for sigma
          area=((naxis1*cdelt1*asec2rad)**2.0)           ! str
          call intnumct(nbin1,nsrc,nsrc1)
          cutoff=2.5+0.5*j
          num_beam=(naxis1*cdelt1/beam)**2.0
          call calcsrcnsigma(cutoff,nbin1,flux,actnoise,nsrc1,num_beam,
     /         n_src,bmpersrc)
          cutoff=2.5+0.5*j ! cutoff in sigma below which pixel is background
          call srcpix(cutoff,lev,cdelt1,beam,levval,flux,actnoise,
     /       nsrc1,npix_src,nbin1)
          call falspix(fpix_false,cutoff,levval)
          x4(j)=cutoff
          y4(j)=dlog10(npix_src(1)/fpix_false(1)/(naxis1*naxis1))
          y41(j)=dlog10(npix_src(2)/fpix_false(2)/(naxis1*naxis1))
          y42(j)=dlog10(npix_src(3)/fpix_false(3)/(naxis1*naxis1))
930      continue
c         call pgsls(i)
c         call pgsci(1)
c         call pgline(7,x4,y4)
c         call pgsci(2)
c         call pgline(7,x4,y41)
c         call pgsci(3)
c         call pgline(7,x4,y42)
920     continue

c        call pgend
c
c  NOW calculate ANGULAR SIZE DISTR FOR HIGHER THAN 5sigma for 1 hr and plot
c
        write (*,'(a46,$)') ' Enter integ time (hrs) for ang size distr'
        read (*,*) dumr   ! integ time in hrs
        do 245 i=1,60     ! ang size in arcsec
         sizeas(i)=i*1.0
         do 246 j=1,4         ! freq
          if (synbeam(j)/asec2rad.gt.sizeas(i)) sizecut(j)=i
246      continue
245     continue
        mini(1)=5.d0
        mini(2)=10.d0
        maxi(1)=10.d0
        maxi(2)=1.d4

cc        call pgbegin(0,'?',2,1)

        do 283 i2=1,2         ! pgpanl
c         call pgbegin(0,'pgplot4.ps/VPS',1,1)  ! new
cc         call pgpanl(i2,1)
c         call pgpanl(1,1)
c         call pgslw(1)
c         call pgsch(1.5)
c         call pgvport(0.15,0.9,0.15,0.9)
c         call pgwindow(0.0,62.0,-1.0,5.0-i2)
c         call pgbox('BCNST',0.0,0,'BCLNST',0.0,0)
c         call pgmove(0.0,0.0)
c         call pgdraw(62.0,0.0)
c         call pgnumb(int(dumr*10),-1,1,str2,nc)
         str1='For > 5\\gs; BW=4 MHz; T\\dint\\u='//str2//' hrs'
c	 call pglabel('Angular size (")','Differential counts',str1)
cc         if (i2.eq.2) call pglabel('Angular size (")',
 
         if (i2.eq.1) then 
cc          call pglabel(' ','Differential counts',str1)
          call plotfundae(0.0,62.0,-1.0,5.0-i2,4)
         end if
         call getlab1(i2,mini,maxi,str1)
c         call pgsch(1.5)
c         call pgtext(10.0,4.5-i2,str1)

        do 250 i=1,4                          ! for freq
         sig5=sens(i)/sqrt(dumr)         ! rms in Jy for 4 MHz BW
         area=2.d0*pi*0.25d0/log(2.0)*pb(i)*pb(i)      ! 2FWHM primary beam in str
         do 253 j=1,60
          hpsi(j)=0.0
253      continue
         do 255 j=1,nbin1
          nsrc(j)=ndiff(j)*area
255      continue
         call intnumct(nbin1,nsrc,nsrc1)      ! get number counts
         do 260 j=1,nbin1-1                   ! now summation over S
          if (fluxf(j,i).ge.sig5*mini(i2).and.
     /        fluxf(j,i).le.sig5*maxi(i2)) then
           do 265 i1=1,60-1
            dumr1=2.d0*((flux(j)*1.d3)**0.3d0)
            dumr=dexp(-log(2.d0)*((sizeas(i1)/dumr1)**0.62))
     /          -dexp(-log(2.d0)*((sizeas(i1+1)/dumr1)**0.62))
            hpsi(i1)=hpsi(i1)+nsrc1(j)*dumr 
265        continue
          end if
260      continue
         do 270 i1=sizecut(i),60-1
          hpsi(i1)=log10(hpsi(i1))
          x4(i1-sizecut(i)+1)=(sizeas(i1)+sizeas(i1+1))/2.d0
          y4(i1-sizecut(i)+1)=hpsi(i1)
270      continue
c         call pgsls(i)
c         call pgline(59-sizecut(i)+1,x4,y4)
250     continue
c        call pgsls(1)
283     continue     ! pgpanl

c        call pgend
c!
c!
c! Now to plot for detecting spectral index at 5 sigma 
c!
c!
c        call pgbegin(0,'pgplot5.ps/VPS',1,2)

c        call pgpanl(1,1)
c        call pgslw(3)
c        call pgsch(1.3)
c        call pgvport(0.15,0.9,0.15,0.9)
c        call pgwindow(-0.2,2.1,-1.0,2.0)
c        call pgbox('BCLNST',0.0,0,'BCLNST',0.0,0)
c        call pglabel('Integration time (hrs)','Minimum flux (mJy)',
 
        do 315 i=2,4
c         call pgsls(i-1)
c         call pgmove(1.4,1.8-(i-2)*0.2)
c         call pgdraw(1.65,1.8-(i-2)*0.2)
315     continue
c        call pgsch(1.0)
c        call pgtext(1.7,1.8,' 75 MHz')
c        call pgtext(1.7,1.6,'120 MHz')
c        call pgtext(1.7,1.4,'200 MHz')
c        call pgsls(1)

        do 305 i=2,4  ! freq
         do 310 j=1,100  ! integ time
          x4(j)=dlog10(j*1.d0)
          y4(j)=dlog10(flux0(j,i)*1.d3)
310      continue
c         call pgsls(i-1)
c         call pgline(100,x4,y4)
c         call pgsls(1)
305     continue

c        call pgpanl(1,2)
c        call pgslw(3)
c        call pgsch(1.3)
c        call pgvport(0.15,0.9,0.15,0.9)
c        call pgwindow(-0.2,2.1,-1.5,0.5)
c        call pgbox('BCLNST',0.0,0,'BCLNST',0.0,0)
c        call pglabel('Integration time (hrs)','Minimum flux (mJy)',
 
        do 330 i=1,4
         do 335 j=1,100  ! integ time
          x4(j)=dlog10(j*1.d0)
          y4(j)=dlog10(flux1(j,i)*1.d3)
335      continue
c         call pgsls(i)
c         call pgline(100,x4,y4)
c         call pgsls(1)
330     continue
        do 331 i=1,4
c         call pgsls(i)
c         call pgmove(1.5,0.6-i*0.2)
c         call pgdraw(1.7,0.6-i*0.2)
c         call pgsls(1)
331     continue
c        call pgsch(1.0)
c        call pgtext(1.8,0.4,'\\ga=-0.6')
c        call pgtext(1.8,0.2,'\\ga=-0.8')
c        call pgtext(1.8,0.0,'\\ga=-1.0')
c        call pgtext(1.8,-0.2,'\\ga=-1.2')
c        call pgsls(1)

c        call pgend



        end


c! assumes circular beam and calculates at pixel centre as aips does
        subroutine lev_val(levs,cdelt,fw,vals)
        implicit none
        real*8 levs(31),cdelt,fw,vals(31),x
        integer i

        do 100 i=1,31
         x=sqrt(levs(i))*cdelt
         vals(i)=dexp(-4.d0*log(2.0)*((x/fw)**2.0))
100     continue

        return
        end


        subroutine addnum(i,n,ns)
        implicit none
        integer i,n
        real*8 ns 
        
        if (i.eq.1) then
         n=n+ns
        else
         n=n+4*ns
        end if

        return
        end


c to calculate nsrc/str * area of image, into an integer
        subroutine intnumct(nbin1,nsrc,nsrc1)
        implicit none
        integer nbin1,i,intn,seed
        real*8 nsrc(1000),nsrc1(1000),random

        seed=-4063
        do 140 i=1,nbin1
         nsrc1(i)=nsrc(i)
         if (nsrc1(i).ge.4.0) then
          nsrc1(i)=int(nsrc1(i))
         else
          intn=int(nsrc1(i))
          nsrc1(i)=nsrc1(i)-intn         ! decimal part of # of src
          call ran1(seed,random)
          if (random.le.nsrc1(i)) then   ! approximate decimal by probability
           nsrc1(i)=intn+1.0             ! if 0.3 srcs then 3/10 times it is 
          else                           ! a src else not one.
           nsrc1(i)=intn*1.0
          end if
         end if
140     continue

        return
        end


        subroutine calcsrcnsigma(ctoff,n1,flux,actnoise,nsrc1,num_beam
     /             ,n_src,bmpersrc)
        implicit none
        real*8 ctoff,flux(1000),actnoise,nsrc1(1000),num_beam,bmpersrc
        integer n1,n_src,i
        
        n_src=0
        do 150 i=1,n1
         if (flux(i).ge.ctoff*actnoise) n_src=n_src+nsrc1(i)
150     continue
        bmpersrc=num_beam/n_src
        if (bmpersrc.lt.40.0) then
         write (*,*) 
         write (*,*) ' *** NEAR CONFUSION LIMIT; WRONG RESULTS ***'
         write (*,*) 
        end if

        return
        end

        subroutine srcpix(cutoff,lev,cdelt1,beam,levval,flux,actnoise,
     /             nsrc1,npix_src,nbin1)
        implicit none
        real*8 cutoff,lev(31),cdelt1,beam,levval(31),flux(1000),actnoise
        real*8 nsrc1(1000)
        integer npix_src(3),i,j,nbin1

        npix_src(1)=0
        npix_src(2)=0
        npix_src(3)=0
        call lev_val(lev,cdelt1,beam,levval)    ! levval is intensity at levels
        do 200 i=1,nbin1                        ! for each src        
         do 210 j=1,31                         
          if (flux(i)*levval(j).ge.cutoff*actnoise) then  ! for peak > cutoff
           call addnum(j,npix_src(1),nsrc1(i))
           if (flux(i)*levval(2).ge.cutoff*actnoise) then  ! 2nd level > cutoff
            call addnum(j,npix_src(2),nsrc1(i)) 
           end if
           if (flux(i)*levval(3).ge.cutoff*actnoise) then  ! 3rd level > cutoff
            call addnum(j,npix_src(3),nsrc1(i)) 
           end if
          end if
210      continue
200     continue
           
        return
        end


        subroutine falspix(fpix_false,cutoff,levval)
        implicit none
        real*8 fpix_false(3),cutoff,levval(31)

        fpix_false(1)=0.5d0*(1.d0-erf(cutoff/levval(1)/sqrt(2.0)))
        fpix_false(2)=0.5d0*(1.d0-erf(cutoff/levval(2)/sqrt(2.0)))
        fpix_false(3)=0.5d0*(1.d0-erf(cutoff/levval(3)/sqrt(2.0)))

        return
        end


        subroutine getlab1(n,mn,mx,st)
        implicit none
        character st*50,st1*10,st2*10
        real*8 mn(2),mx(2)
        integer n,nc1,nc2

c        call pgnumb(int(mn(n)*10),-1,1,st1,nc1)
        st1=st1(1:nc1)//'\\gs to '
c        call pgnumb(int(mx(n)*10),-1,1,st2,nc2)
        st=st1//st2(1:nc2)//'\\gs'

        return
        end



        subroutine plotfundae(mnx,mxx,mny,mxy,n)
        implicit none
	integer n,i
	real*4 mnx,mxx,mny,mxy,dum

	dum=0.06
	do 100 i=1,n
c         call pgmove(mnx+0.05*(mxx-mnx),mxy-i*dum*(mxy-mny))
c	 call pgsls(i)
c         call pgdraw(mnx+0.20*(mxx-mnx),mxy-i*dum*(mxy-mny))
100     continue
c        call pgsls(1)
c	call pgtext(mnx+0.22*(mxx-mnx),mxy-dum*1*(mxy-mny),' 30 MHz')
c	call pgtext(mnx+0.22*(mxx-mnx),mxy-dum*2*(mxy-mny),' 75 MHz')
c	call pgtext(mnx+0.22*(mxx-mnx),mxy-dum*3*(mxy-mny),'120 MHz')
c	call pgtext(mnx+0.22*(mxx-mnx),mxy-dum*4*(mxy-mny),'200 MHz')

        return
        end


c!
c! Fit in log space and get n (like nick does)
c!
        subroutine nicknum(fl,numb)
	implicit none
	character filen*8,dir*500
	integer num,i,ia(5)
	real*8 s(200),no(200),x(34),y(34),sig1(34),dumr
	real*8 a(5),covar(5,5),alpha(5,5),chisq,alamda,s1
	real*8 ex,why,fac,y1(200),yold,xold,fl,numb,r1,r2
	real*8 xf(27),yf(27),sig(27)
	real*4 x4(100),y4(100),y41(100),y42(100)

	data ia/1,1,1,1,1/
	data a/0.41d0,-2.52,-0.06,0.0,0.0/

c! cant extrapolate to higher flux properly for low freq which needs greater 
c! flux than in nick.sub so extend by log-log and then come back to this.

	filen='nick.sub'
        dir="./"
	call getline(filen,dir,num)
	open(unit=21,file=filen,status='old')
 	do 400 i=1,num
         read (21,*) x(i),y(i)
         xf(i)=x(i)-3.d0
	 yf(i)=dlog10((10.d0**y(i))/((10.d0**xf(i))**2.5d0))
	 sig(i)=1.d-4
400     continue
	close(21)
	alamda=-1.d0
        do 420 i=1,20
         call mrqmin(xf,yf,sig,num,num,a,ia,5,covar,alpha,5,chisq,
     /       alamda,3)
420     continue
        alamda=0.d0
        call mrqmin(xf,yf,sig,num,num,a,ia,5,covar,alpha,5,chisq,
     /       alamda,3)

c! newnick has log(S) and log(dN/dS) in str^-1 with 7 extra values
	filen='newnick'
	open(unit=21,file=filen,status='unknown')
	do 435 i=1,num
	 write (21,*) xf(i),a(1)+a(2)*xf(i)+a(3)*xf(i)*xf(i)
     /    +a(4)*(xf(i)**3.d0)+a(5)*(xf(i)**4.d0)
435     continue
        do 432 i=1,7
         x(i)=xf(num)+0.079*i
	 y(i)=a(1)+a(2)*x(i)+a(3)*x(i)*x(i)
     /    +a(4)*(x(i)**3.d0)+a(5)*(x(i)**4.d0)
         write (21,*) x(i),y(i)
432     continue
        close(21)
	num=num+7
          
	filen='newnick'
	open(unit=21,file=filen,status='old')
 	do 100 i=1,num
	 read (21,*) x(i),y(i)
	 s(i)=x(i)    ! logS in Jy
	 no(i)=(10.d0**(y(i)+s(i)))   ! dN/dS * S
	 sig1(i)=0.001
100     continue
	close(21)
c!
c! now calc in log space like nick says and compare
c!
	alamda=-1.d0
        do 220 i=1,20
         call mrqmin(s,no,sig1,num,num,a,ia,5,covar,alpha,5,chisq,
     /       alamda,5)
220     continue
        alamda=0.d0
        call mrqmin(s,no,sig1,num,num,a,ia,5,covar,alpha,5,chisq,
     /       alamda,5)

        s1=0.d0
	r1=log10(fl)
	r2=s(num)
        fac=(r2-r1)/30000.d0
        do 200 i=1,30000
         ex=r1+(i-1)*fac
 	 why=a(1)+a(2)*ex+a(3)*ex*ex+a(4)*(ex**3.d0)+a(5)*(ex**4.d0)
	 if (i.gt.1) then
	  s1=s1+0.5d0*(why+yold)*fac
	 end if
         yold=why 
200     continue
        s1=s1*log(10.0)
        numb=s1
	if (-0.8.ge.log10(fl)) write (*,*) '  ### ERROR ###'

c 	call pgend


        return
        end




