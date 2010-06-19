c! this is to test confusion noise formalism of condon thro simulating images.
c! rather, to test my understanding of it.
c! g77 testconfusionnoise.f clickop.f sigclip.f ran1.f arrstat.f sub_prog.f 
c! -o testconfusionnoise -L/usr/lib -L/usr/X11R6/lib -lpng -lX11 -L$PGPLOT_DIR -lpgplot
c!
c! after talking to condon, rewriting this. see his notes and my notes. also resampling is done 
c! sensibly now than creating larger image first !
c! ok, so for q=3.5 or so, we get kappa clipped rms.
c!
c! now modify for
c! source not at pixel centre and 
c! also beam bigger than 1 pixel to verify how to calculate.
c! note that this is for confusion noise and blending will need the full
c! rms calculated by assuming D_c = maximum source flux or maybe n*total_rms
c!
c! Not doing confusion noise image due to primary beam.

        implicit none
        include "constants.inc"
        real*8 k1,gam,smin,smax,freq,nyqfac,maxdia,k2
        real*8 minavgspp,g1,dumr,fl,fu,minper
        integer n,m,i,writea,seed,nf,id1,id2,pgopen,j,nchar
        real*4 x1(30),y1(30),x2(30),y2(30),mn,mx
        real*4 krms(10,30),calcq(10,30)   ! kappa,freq
        character solnname*500
        
        write (*,*) 
     /      '   Simulates confusion noise for a well sampled image'
        write (*,*) '   Assumes diff src cts as 277 * S^(-2.17)'
        write (*,*) '   downto 0 flux (in practice, 1e-8 Jy) '
        write (*,*) 
        write (*,*) '   Enter L-band k and gamma {n~s^(-gamma)}   '
        write (*,'(a,$)') '         (recommended 277.0, 2.17) : '
c        read (*,*) k1,gam
        k1=277.d0
        gam=2.17d0
        smin=1.d-8  ! Jy
        smax=1.d3   ! Jy
        write (*,'(a)') '   Size of image (n,m) : '
c        read (*,*) n,m
        n=512
        m=n
        write (*,'(a)') '   Max effective baseline (km) : '
c!       read (*,*) maxdia
        maxdia=50.d0
        write (*,'(a)') '   Factor times Nyquist sampling : '
        read (*,*) nyqfac
c        nyqfac=2.d0 ! factor times 2 pixels per fwhm of beam
        write (*,'(a)') '   Freq_low, freq_hi, # freq (MHz) : '
        read (*,*) fl,fu,nf
        if (fl.gt.fu) call xchangr8(fl,fu)
        write (*,'(a)') '   Min fraction of zeroes in image : '
        read (*,*) minper
        write (*,*) '  Enter solnname'
        read(*,*) solnname

        writea=1
        call getseed(seed)
        g1=1.d0-gam
        do i=1,nf
         freq=fl+(i-1)*(fu-fl)/(nf-1)  ! MHz
         k2=k1*((freq/1.4d3)**(-0.7d0))
         dumr=(k2/g1)*((c/(freq*1.d6*maxdia*1.d3*2*nyqfac))**2.d0)
         smin=(smax**g1 + dlog(minper)/dumr)**(1.d0/g1)
         call sub_testconfusionnoise(k1,gam,freq,smin,smax,n,m,nyqfac,
     /       writea,seed,maxdia,x1,y1,x2,y2,i,krms,calcq,10,30)
        end do

        id1=pgopen('conf_fullrms_vs_freq_'//solnname(1:nchar(solnname))
     /      //'.ps/VPS')
        mn=log10(fl)*0.9
        mx=log10(fu)*1.1
        call pgslct(id1)
        call pgvport(0.1,0.9,0.1,0.9)
        call pgsch(1.3)
        call pgslw(2)
        call pgwindow(mn,mx,0.0,3.0)
        call pgbox('BCNLST',0.0,0,'BCNST',0.0,0)
        call pglabel('Frequency (MHz)','RMS ratio (Dc=max/arrstat)','')
        call pgpoint(nf,x1,y1,16)
        call pgclos
        id1=pgopen('conf_k_vs_q_'//solnname(1:nchar(solnname))//
     /      '.ps/VCPS') 
        call pgslct(id1)
        call pgsch(1.3)
        call pgslw(2)
        call pgvport(0.1,0.9,0.1,0.9)
        call pgwindow(1.0,7.5,1.0,7.5)
        call pgbox('BCNST',0.0,0,'BCNST',0.0,0)
        call pglabel('Kappa','Calculated q','')
        call pgmove(min(1.0,1.0),min(1.0,1.0))
        call pgdraw(max(7.5,7.5),max(7.5,7.5))
        do i=1,nf
         do j=1,10
          x1(j)=2.0+0.50*(j-1)
          y1(j)=calcq(j,i)
         end do
         call pgsci(i)
         call pgpoint(10,x1,y1,16)
         call pgline(10,x1,y1)
         call pgsci(1)
        end do
          write (*,*) 
        call pgclos
        id1=pgopen('conf_k_vs_krms_'//solnname(1:nchar(solnname))//
     /      '.ps/VCPS')
        call pgslct(id1)
        call pgsch(1.3)
        call pgslw(2)
        call pgvport(0.1,0.9,0.1,0.9)
        call pgwindow(1.0,7.5,1.0,8.5)
        call pgbox('BCNST',0.0,0,'BCNST',0.0,0)
        call pglabel('Kappa','Normalised kappa-clipped rms with offset'
     /        ,'')
        do i=1,nf
         do j=1,10
          x1(j)=2.0+0.5*(j-1)
          y1(j)=krms(j,i)/krms(1,i)+0.5*i
          end do
          call pgsci(i)
          call pgline(10,x1,y1)
          call pgsci(1)
        end do
        call pgclos
        
        return
        end

        subroutine sub_testconfusionnoise(k,gam,freq,smin,smax,
     /     n,m,nyqfac,writea,seed,maxdia,x1,y1,x2,y2,ifreq,
     /     krms,calcq,n1,n2)
        implicit none
        include "constants.inc"
        integer n,m,totsrc,i,seed,j,ind1,ind2,nchar
        integer writea,q,x,y,ii,jj,subsize,round,ifreq,n1,n2
        real*8 k1,gam,smin,smax,freq,image(n,m),nums(n,m),nyqfac
        real*8 dr1,dr2,area,nsrc,g1,rand,flux,k,kappa
        real*8 onesrc,fac,wid,fac1,bmfwhm,avgspp,dumr1
        real*8 sig_c(3),sigma,std,av,num,cdelt,imsize
        real*8 stdc,avc,maxbin,maxdia,xc,yc,sigpix
        real*8 omega_e,Dc1,Dc2,sig1,sig2,numbers,dumr,s1,s2
        real*4 x1(30),y1(30),x2(30),y2(30)
        real*4 krms(n1,n2),calcq(n1,n2)   
        character lab1*500,f1*500,f2*500,extn*20
        character bcon*10,bcode*4,str10*10
        logical toobig

        k1=k*((freq/1400.d0)**(-0.7d0))

        g1=1.d0-gam
        dr1=k1/g1*(smin**g1)
        dr2=k1/g1*(smax**g1)

        call initialiseimage(image,n,m,n,m,0.d0)
        call initialiseimage(nums,n,m,n,m,0.d0)

c! area of pixel on which rms is calculated on the image must be same as
c! area going into omega_e so take pixel area
        nsrc=k1/g1*(smax**g1-smin**g1)     ! num of srcs/str
        bmfwhm=c/(freq*1.d6)/(maxdia*1.d3)*rad   ! deg; assume for now
        cdelt=bmfwhm*3600.d0/(2.d0*nyqfac)  ! "/pix
        sigpix=bmfwhm/2.35d0*3600.d0/cdelt
        imsize=n*cdelt ! image size in "
        area=imsize*imsize/3600.d0/3600.d0/rad/rad ! str
        write (*,*) '  Imaging ',(50.d0/maxdia/2.d3*n/nyqfac)**2.d0,
     /              ' of primary beam'
        if (nsrc*area.gt.1.d9) then
         toobig=.true.
        else
         toobig=.false.
         totsrc=int(nsrc*area)  ! num of srcs in area
        end if
        write (*,*) '  Frequency = ',freq,' MHz'
        write (*,*) '  Total number of srcs = ',nsrc*area
        write (*,*) '  Average number of srcs/pix = ',nsrc*area/n/n
        write (*,*) '  Size of 1 pixel = ',cdelt,' "'

        avgspp=(k1/g1*(smax**g1-smin**g1))*
     /     ((c/(freq*1.d6*maxdia*1.d3*2*nyqfac))**2.d0)
        write (*,*) '  # pix with zero sources  = ',n*n*dexp(-avgspp)

c! estimate rms0
        omega_e=area/(n*n)
        omega_e=0.25d0*pi*bmfwhm*bmfwhm/rad/rad/(dlog(2.d0)*(gam-1.d0)) ! correct area
        write (*,*) '  Omega_e = ',omega_e

        maxbin=-9.d90
        if (.not.toobig) then
         ind1=totsrc
         ind2=1
        else
         fac1=10.d0**(int(dlog10(nsrc*area/1.d9))+1)
         ind1=int(nsrc*area/fac1)
         ind2=fac1
        end if
        subsize=round(bmfwhm*3600.d0*8.d0/cdelt)

        do j=1,ind2
         do i=1,ind1
          call ran1(seed,rand)
          flux=((dr1-rand*(dr1-dr2))*g1/k1)**(1.d0/g1)
          call ran1(seed,rand)
          xc=rand*(n-1)+1
          call ran1(seed,rand)
          yc=rand*(n-1)+1
          do ii=1,subsize
           do jj=1,subsize
            x=int(xc-subsize/2.d0)+(ii-1)
            y=int(yc-subsize/2.d0)+(jj-1)
            dumr=((x-xc)*(x-xc)+(y-yc)*(y-yc))/
     /           (2.d0*sigpix*sigpix)
            if (x.ge.1.and.x.le.n.and.y.ge.1.and.y.le.n) then
             image(x,y)=image(x,y)+flux*dexp(-dumr)
            end if
           end do
          end do
          nums(int(xc),int(yc))=nums(int(xc),int(yc))+1.d0
          if (flux.gt.maxbin) maxbin=flux
          if (mod(i,ind1/100).eq.0) write (*,'(a1,$)') '.'
         end do
        end do

        s1=0.d0
        s2=0.d0
        do ii=1,subsize
         do jj=1,subsize
          x=int(xc-subsize/2.d0)+(ii-1)
          y=int(yc-subsize/2.d0)+(jj-1)
          dumr=((x-xc)*(x-xc)+(y-yc)*(y-yc))/
     /         (2.d0*sigpix*sigpix)
          s1=s1+dexp(-dumr)
          s2=s2+dexp(-dumr)*dexp(-dumr)
         end do
        end do

        if (writea.eq.1) then
         call int2str(int(freq),str10)
c         f2='im1.nums_'//str10(1:nchar(str10))//'MHz'
c         call writearray_bin2D(nums,n,m,n,m,f2,'mv')
c         f1='conf_'//str10(1:nchar(str10))//'MHz'
c         call writearray_bin2D(image,n,m,n,m,f1,'mv')
        end if
        call arrstat(image,n,m,1,1,n,m,std,av)

c! calc condon
        Dc2=maxbin
        sig1=sqrt(k1*omega_e/(3.d0-gam))*(Dc2**(1.5d0-0.5d0*gam))
        sig2=(k1*omega_e*(q**(3.d0-gam))/(3.d0-gam))**(1.d0/(gam-1.d0))
        x1(ifreq)=log10(freq)
        x2(ifreq)=log10(freq)
        y1(ifreq)=std/sig1
         write (*,*) x1(ifreq),y1(ifreq),std,sig1
        do i=1,n1
         kappa=2.d0+0.5d0*(i-1)
         call sigclip(image,n,m,1,1,n,m,stdc,avc,kappa)
         calcq(i,ifreq)=((3.d0-gam)/(k1*omega_e)*(stdc**(gam-1.d0)))**
     /       (1.d0/(3.d0-gam))  ! =q
         krms(i,ifreq)=stdc
        end do

        return
        end


