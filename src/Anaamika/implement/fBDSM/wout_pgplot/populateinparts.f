c! populate image with given src distr fn in parts. 500 divided in 
c! sections of length nbin
c! cn1 and gam1 are for each section. 

        subroutine populateinparts(sarr,cdfarr,nbin,cn1,gam1,
     /             n,m,f1,freq,pixsc,seed,srldir)
        implicit none
        integer n,m,nbin,i,j,seed,x,y,iareapix,xpix(100),ypix(100)
        real*8 array(n,m),sarr(500),cdfarr(500),cn1(500),gam1(500)
        integer npixl,nchar
        real*8 rand,dumr1,dumr2,nsrc,flux,std,av,arr5sig(n,m)
        real*8 pixsc,freq,medsize,sizeas,areapix,confn
        character strdev*5,lab*500,f1*(*),lab2*500,filen*500,dir*500
        character srldir*500
        real*4 dfl

        call getconfn4nu(freq,confn)

        call initialiseimage(array,n,m,n,m,0.d0)     ! makes a zero image
        write (*,*) '  NOTE: Source peaks are assumed at pixel centres'
        write (*,*) '  NOTE: Extended sources not put in properly'
        filen='pixels'
        dir="./"
        call getline(filen,dir,npixl)
        open(unit=21,file=filen(1:nchar(filen)),status='old')
        do 115 i=1,npixl
         read (21,*) xpix(i),ypix(i)
115     continue
        close(21)

        dumr1=cn1(1)*(sarr(1)**gam1(1))
        dumr2=cn1(500/nbin)*(sarr(500)**gam1(500/nbin))
        write (*,*) '  Putting in ',dumr1-dumr2,' sources ... '
        open(unit=21,file=srldir(1:nchar(srldir))//'srclist',
     /       form='unformatted')
        do 100 i=1,500/nbin 
        
         dumr1=cn1(i)*(sarr((i-1)*nbin+1)**gam1(i))
         if ((i*nbin+1).le.500) then 
          dumr2=cn1(i+1)*(sarr(i*nbin+1)**gam1(i+1))
         else
          dumr2=cn1(500/nbin)*(sarr(500)**gam1(500/nbin))
         end if
         nsrc=dumr1-dumr2
c         if (sarr(i*nbin+1).lt.confn*0.01d0) goto 555
         if (i.eq.500/nbin.and.dumr2.gt.0.1d0) then
          write (*,*)
          write (*,*) '  ### ',dumr2,' sources remaining in bin ',i
          write (*,*) '  ### Fix It !!!'
         end if
         if ((i*nbin+1).le.500) write (*,*) ' ',nsrc,' srcs bet ',
     /    sarr((i-1)*nbin+1)/4.6d-5,' & ',sarr(i*nbin+1)/4.6d-5

c!  ERROR -- putting all sources at pixel centres ! modify later to either oversample
c!  or have multiple components of both signs. 
c!  ERROR -- for extended sources, put total flux/num pix over int(num pixels)

         do 110 j=1,int(nsrc)    
          call ran1(seed,rand)
          call dfunc(dumr1,dumr2,cn1(i),gam1(i),rand,flux)  ! flux has correct p.d. now
          call getrandposn(n,m,x,y,seed)
          medsize=2.d0*(flux*1.d3*((1.4d9/freq)**(-0.8d0)))**0.3d0  ! median size in arcsec
          call ran1(seed,rand)
          call calcsize(seed,medsize,sizeas)  ! sizeas is actual size in arcsec
          areapix=(sizeas/pixsc)**2.d0   ! area of extended source in pixels
          iareapix=int(areapix)
          if (areapix-iareapix.gt.0.5d0) iareapix=iareapix+1
          if (iareapix.eq.0) iareapix=1
c          call badputsource(array,x,y,iareapix,flux,npixl,xpix,ypix)
          array(x,y)=array(x,y)+flux
          dfl=dlog10(flux)
          write (21) dfl
          if (nsrc.gt.5.d6.and.mod(j,1000000).eq.0) 
     /       write (*,'(a1,$)') '.'
110      continue
555      continue  ! fudge. skip is lower limit of bin < conf_noise/100
100     continue
        write (*,*)
        close(21)

        strdev='/xs'
        lab='Image of sources'
        lab2='SNR image of > 5 sigma pixels'
        call arrstat(array,n,m,1,1,n,m,std,av)
        write (*,'(1x,a14,1Pe9.2,a14,1Pe9.2,a4)') 
     /     '  Image std = ',std*1.d3,' mJy ; mean = ',av*1.d3,' mJy'
        i=n*m
        call sigclip(array,n,m,1,1,n,m,std,av,5.d0)
        write (*,'(1x,a19,1Pe9.2,a14,1Pe9.2,a4)') 
     /   '  5-sig clip std = ',std*1.d3,' mJy ; mean = ',av*1.d3,' mJy'

        do 200 j=1,m
         do 210 i=1,n
          if (array(i,j)-av.lt.5.d0*std) then
           arr5sig(i,j)=0.d0  
          else
           arr5sig(i,j)=array(i,j)/std  ! SNR for 5-sigma clipped std
          end if
210      continue
200     continue
        
        call grey2(array,arr5sig,n,m,n,m,strdev,lab,lab2,0,1)

        call writearray_bin2D(array,n,m,n,m,f1,'mv')

        return
        end



        subroutine dfunc(n_ga,n_gb,cn,gam,rand,func)
        implicit none
        real*8 n_ga,n_gb,cn,gam,rand,func

        func=((n_ga-(n_ga-n_gb)*rand)/cn)**(1.d0/gam)
        
        return
        end


        subroutine getrandposn(n,m,x,y,seed)
        implicit none
        integer n,m,x,y,seed
        real*8 rand

        call ran1(seed,rand)
        x=int(rand*(n-1)+1)
        call ran1(seed,rand)
        y=int(rand*(m-1)+1)
        
        return
        end



        subroutine calcsize(seed,medsize,sizeas)  ! sizeas is actual size in arcsec
        implicit none
        integer seed
        real*8 medsize,sizeas,a,b,rand,h,ha,hb

        a=min(medsize*0.2d0,0.5d0)   ! min arcsec
        b=40.d0 ! max arcsec

        call ran1(seed,rand)

        ha=h(medsize,a)
        hb=h(medsize,b)
        sizeas=-dlog(rand*(hb-ha)+ha)
        sizeas=medsize*((sizeas/dlog(2.d0))**(1.d0/0.62d0))

        return
        end


        function h(med,val)
        implicit none
        real*8 med,val,h
        
        h=dexp((-1.d0)*dlog(2.d0)*((val/med)**(0.62d0)))

        return
        end

c! bad because u just divide total flux by int(area in pixel) and put it equally.
c! assymetrically. shud actually convolve with pixel and put. later. 
        subroutine badputsource(array,x,y,area,flux,np,xp,yp)
        implicit none
        integer x,y,area,np,xp(100),yp(100),i
        real*8 array(x,y),flux,fluxpp

        fluxpp=flux/area
        if (np.lt.area) then
         write (*,*) '  ###  Source too big. Modify file pixels'
        end if

        do 100 i=1,area
         array(x+xp(i),y+yp(i))=fluxpp
100     continue

        return
        end




        subroutine getconfn4nu(freq,confn)
        implicit none
        real*8 freq,confn

        if (freq.eq.30.d0) confn=250.d-6
        if (freq.eq.75.d0) confn=30.d-6
        if (freq.eq.120.d0) confn=10.d-6
        if (freq.eq.200.d0) confn=3.d-6

        return
        end



