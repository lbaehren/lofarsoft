c! this is to test confusion noise formalism of condon thro simulating images.
c! rather, to test my understanding of it.
c! g77 testconfusionnoise.f clickop.f sigclip.f ran1.f arrstat.f sub_prog.f 
c! -o testconfusionnoise -L/usr/lib -L/usr/X11R6/lib -lpng -lX11 -L$PGPLOT_DIR -lpgplot
c!
c! after talking to condon, rewriting this. see his notes and my notes. also resampling is done 
c! sensibly now than creating larger image first !

        implicit none
        include "constants.inc"
        real*8 k1,gam,smin,smax,freq,nyqfac,s(10),k2,g1
        real*8 fl,fu,dumr,minper
        integer n,m,nbin,i,writea,seed,id1,id2,pgopen,nf
        real*4 x1(10),y1(10),x2(10),y2(10),mn,mx
        
        write (*,*) '   Enter L-band k and gamma {n~s^(-gamma)}   '
        write (*,'(a,$)') '         (recommended 277.0, 2.17) : '
c        read (*,*) k1,gam
        k1=277.d0
        gam=2.17d0
        smin=1.d-8  ! Jy
        smax=1.d0   ! Jy
        nyqfac=1.d0 
        n=1024
        m=n
        write (*,'(a)') '   Size of image (n,m) : '
        write (*,'(a)') '   Factor times Nyquist sampling : '
c        read (*,*) nyqfac

        writea=1
        nbin=10
        s(1)=3.d-7
        s(2)=1.d-6
        s(3)=3.d-6
        s(4)=1.d-5
        s(5)=3.d-5
        s(6)=1.d-4
        s(7)=1.d-3
        s(8)=1.d-2
        s(9)=1.d-1
        s(10)=smax
        g1=1.d0-gam
        minper=0.00001d0

        call getseed(seed)

        write (*,'(a)') '   Freq_low, freq_hi, # freq (MHz) : '
        read (*,*) fl,fu,nf
        if (fl.gt.fu) call xchangr8(fl,fu)

        id1=pgopen('a.ps/VPS')
        call pgslct(id1)
        call pgsch(1.3)
        call pgslw(2)
        call pgvport(0.1,0.9,0.1,0.9)
        call pgwindow(-7.0,1.5,0.5,1.5)
        call pgbox('BCNLST',0.0,0,'BCNST',0.0,0)
        call pglabel('Max deflection D_c','sig_condon/sig_image',' ')
        do i=1,nf
         freq=fl+(i-1)*(fu-fl)/(nf-1)  ! MHz
         k2=k1*((freq/1.4d3)**(-0.7d0))
         dumr=(k2/g1)*((c/(freq*1.d6*50.d0*1.d3))**2.d0)
         smin=(smax**g1 + dlog(minper)/dumr)**(1.d0/g1)
         call pgsci(11-i)
         call sub_testconfusionnoise(k1,gam,freq,smin,smax,n,m,nyqfac,
     /       nbin,s,writea,seed,id1,x1,x2,y1,y2,i)
        end do
        call pgslct(id1)
        call pgclos

        mn=log10(fl)*0.9
        mx=log10(fu)*1.1
        id1=pgopen('confusion_rmsratiofull_nobeam_freq.ps/VPS')
        call pgslct(id1)
        call pgvport(0.1,0.9,0.1,0.9)
        call pgsch(1.3)
        call pgslw(2)
        call pgwindow(mn,mx,0.5,1.5)
        call pgbox('BCNLST',0.0,0,'BCNST',0.0,0)
        call pglabel('Frequency (MHz)','RMS ratio (Dc=max/arrstat)','')
        call pgpoint(nf,x1,y1,16)
        call pgclos
        id1=pgopen('confusion_q_nobeam_freq.ps/VPS')
        call pgslct(id1)
        call pgvport(0.1,0.9,0.1,0.9)
        call pgsch(1.3)
        call pgslw(2)
        call pgwindow(mn,mx,3.0,4.0)
        call pgbox('BCNLST',0.0,0,'BCNST',0.0,0)
        call pglabel('Frequency (MHz)','Calculated q','')
        call pgpoint(nf,x2,y2,16)
        call pgclos
        
        return
        end

        subroutine sub_testconfusionnoise(k1,gam,freq,smin,smax,
     /    n,m,nyqfac,nbin,s,writea,seed,id1,x1,x2,y1,y2,id)
        implicit none
        include "constants.inc"
        integer n,m,totsrc,i,seed,x,y,j,ind1,ind2,nbin,ii,nchar
        integer writea,id1,id
        real*8 k1,gam,smin,smax,freq,image(n,m),nums(n,m),nyqfac
        real*8 dr1,dr2,area,nsrc,g1,rand,flux,s(nbin),k2
        real*8 pbtheta,onesrc,arr5sig(n,m),s1,fac,wid,fac1
        real*8 sig_c(3),sigma,std(nbin),av(nbin),num(nbin)
        real*8 stdc,avc,q,maxdia,avgspp
        real*8 imcube(n,m,nbin),ch0(n,m),maxbin(nbin)
        real*8 omega_e,Dc1,Dc2,sig1,sig2,numbers(nbin),dumr
        real*4 x4,y4
        real*4 x1(10),y1(10),x2(10),y2(10)
        character lab1*500,lab2*500,f1*500,f2*500,extn*20
        character strdev*5,bcon*10,bcode*4,lab*500,str10*10
        logical toobig

        q=4.d0
        maxdia=50.d0
        k2=k1*((freq/1400.d0)**(-0.7d0))

        g1=1.d0-gam
        dr1=k2/g1*(smin**g1)
        dr2=k2/g1*(smax**g1)

        call initialiseimage3d(imcube,n,m,nbin,n,m,nbin,0.d0)
        call initialiseimage(image,n,m,n,m,0.d0)
        call initialiseimage(nums,n,m,n,m,0.d0)
        call initialise_vec(num,nbin,0)

c! area of pixel on which rms is calculated on the image must be same as
c! area going into omega_e. so take pixel area
        nsrc=k2/g1*(smax**g1-smin**g1)     ! num of srcs/str
        write (*,*) 'one pixel per beam'
        pbtheta=3.d8/(freq*1.d6)/maxdia ! is size of image in rad
        area=pbtheta*pbtheta ! n pixels each pbtheta/n radian
        if (nsrc*area.gt.1.d9) then
         toobig=.true.
        else
         toobig=.false.
         totsrc=int(nsrc*area)  ! num of srcs in area
        end if
        write (*,*) ' Total number of srcs = ',nsrc
        write (*,*) ' Average number of srcs/pix = ',nsrc*area/n/n
        write (*,*) '  Size of 1 pixel = ',
     /        pbtheta/(n*1.0)*180.0/3.1415*3600.0,' "'   ! arcsec
        write (*,*) '  Area of 1 pixel = ',((pbtheta/(n*1.0))**2.d0),
     /        ' str'
        avgspp=(k2/g1*(smax**g1-smin**g1))*
     /     ((c/(freq*1.d6*maxdia*1.d3))**2.d0)
        write (*,*) '  # pix with zero sources  = ',n*n*dexp(-avgspp)

        if (.not.toobig) then
         ind1=totsrc
         ind2=1
        else
         fac1=10.d0**(int(dlog10(nsrc*area/1.d9))+1)
         ind1=int(nsrc*area/fac1)
         ind2=fac1
        end if
        do j=1,ind2
         do i=1,ind1
          call ran1(seed,rand)
          flux=((dr1-rand*(dr1-dr2))*g1/k2)**(1.d0/g1)
          call ran1(seed,rand)
          x=int(rand*(n-1)+1)
          call ran1(seed,rand)
          y=int(rand*(n-1)+1)
          image(x,y)=image(x,y)+flux
          nums(x,y)=nums(x,y)+1.d0
          do ii=1,nbin
           if (flux.le.s(ii)) then 
            imcube(x,y,ii)=imcube(x,y,ii)+flux
            num(ii)=num(ii)+1
            if (flux.gt.maxbin(ii)) maxbin(ii)=flux
           end if
          end do
          if (mod(i,ind1/100).eq.0) write (*,'(a1,$)') '.'
         end do
        end do
        write (*,*) 

        strdev='/xs'
        lab='Image of sources'
        lab2='SNR image of > 5 sigma pixels'

        if (writea.eq.1) then
         call int2str(int(freq),str10)
c         f2='im1.nums_'//str10(1:nchar(str10))//'MHz'
c         call writearray_bin2D(nums,n,m,n,m,f2,'mq')
c         f1='conf_pix_'//str10(1:nchar(str10))//'MHz'
c         call writearray_bin2D(image,n,m,n,m,f1,'mq')
        end if
        do ii=1,nbin
         do j=1,m
          do i=1,n
           ch0(i,j)=imcube(i,j,ii)
          end do
         end do
         call arrstat(ch0,n,m,1,1,n,m,std(ii),av(ii))
         if (writea.eq.1) then
          call int2str(ii,str10)
          call numE2str(s(ii),str10)
          f1='conf_'//str10(1:nchar(str10))
          call int2str(int(freq),str10)
          f1=f1(1:nchar(f1))//'_'//str10(1:nchar(str10))//'MHz'
c          call writearray_bin2D(ch0,n,m,n,m,f1,'mq')
         end if
        end do
        call sigclip(image,n,m,1,1,n,m,stdc,avc,q)

c! calc condon
        area=((pbtheta/(n*1.0))**2.d0)!*(gam-1.d0)   ! area is for one pixel for flat beam

        g1=1.d0/(gam-1.d0)
        dr1=((k2*area)**g1)/((3.d0-gam)**g1)
        dr2=(3.d0-gam)*g1

        do i=1,nbin
         if (i.eq.1) then
          numbers(i)=num(1)
         else
          numbers(i)=num(i)-num(i-1)
         end if
c         write (*,*)
c     /     num(i),' sources bet 1e-7 & ',s(i),' (+',numbers(i),')'
        end do

        do i=1,nbin
         omega_e=((pbtheta/(n*1.0))**2.d0)
         Dc1=s(i)
         Dc2=maxbin(i)
         sig1=sqrt(k2*omega_e/(3.d0-gam))*(Dc1**(1.5d0-0.5d0*gam))
         sig2=sqrt(k2*omega_e/(3.d0-gam))*(Dc2**(1.5d0-0.5d0*gam))
         x4=log10(Dc2)
         y4=sig2/std(i)
c         write (*,*) freq,Dc2,sig2,std(i),Dc2/sig2
         call pgslct(id1)
         if (numbers(i).gt.10.d0) then
          if (nsrc*area/n/n.gt.1.d0) then
           call pgsch(1.5)
           call pgpoint(1,x4,y4,16)
           call pgsch(1.0)
          else
           call pgsch(1.5)
           call pgpoint(1,x4,y4,23)
           call pgsch(1.0)
          end if
         else
          call pgpoint(1,x4,y4,6)
          if (nsrc*area/n/n.le.1.d0) then
           call pgpoint(1,x4,y4,23)
          end if
         end if
        end do
        x1(id)=log10(freq)
        x2(id)=log10(freq)
        y1(id)=(sqrt(k2*omega_e/(3.d0-gam))*(maxbin(nbin)**
     /     (1.5d0-0.5d0*gam)))/std(nbin)
          write (*,*) id,x1(id),y1(id),
     /     (sqrt(k2*omega_e/(3.d0-gam))*(maxbin(nbin)**
     /     (1.5d0-0.5d0*gam))),std(nbin)
        y2(id)=((3.d0-gam)/(k2*omega_e)*(stdc**(gam-1.d0)))**
     /       (1.d0/(3.d0-gam))  ! =q
          write (*,*) id,x2(id),y2(id)

        return
        end


