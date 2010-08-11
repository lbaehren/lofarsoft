c! this is to test confusion noise formalism of condon thro simulating images.
c! rather, to test my understanding of it.
c! g77 testconfusionnoise.f clickop.f sigclip.f ran1.f arrstat.f sub_prog.f -o testconfusionnoise -L/usr/lib -L/usr/X11R6/lib -lpng -lX11 -L$PGPLOT_DIR -lpgplot
c!
c! after talking to condon, rewriting this. see his notes and my notes. also resampling is done 
c! sensibly now than creating larger image first !

        implicit none
        real*8 k,gam,smin,smax,freq,nyqfac
        integer n,m
        
        write (*,*) '   Enter L-band k and gamma {n~s^(-gamma)}   '
        write (*,'(a,$)') '         (recommended 277.0, 2.17) : '
        read (*,*) k,gam
        write (*,'(a,$)') '   Frequency in MHz : '
        read (*,*) freq
        write (*,'(a,f5.1,a,$)') '   S_min and S_max at ',freq,' MHz : '
333     read (*,*) smin,smax
        if (smin.eq.smax) goto 333
        if (smin.gt.smax) call xchangr8(smin,smax)
        write (*,'(a,$)') '   Size of image (n,m) : '
        read (*,*) n,m
        write (*,'(a,$)') '   Factor times Nyquist sampling : '
        read (*,*) nyqfac

        call sub_testconfusionnoise(k,gam,freq,smin,smax,n,m,nyqfac)
        
        return
        end

        subroutine sub_testconfusionnoise(k,gam,freq,smin,smax,
     /             n,m,nyqfac)
        implicit none
        integer n,m,totsrc,i,seed,x,y,j
        real*8 k,gam,smin,smax,freq,image(n,m),nums(n,m),nyqfac
        real*8 dr1,dr2,area,nsrc,g1,rand,flux,array(n,m)
        real*8 pbtheta,std,av,onesrc,arr5sig(n,m),s1,fac,wid,fac1
        real*8 sig_c(3),sigma,carray(n,m),arr2(n,m),rarr(n,m)
        character lab1*500,lab2*500,f1*500,f2*500,extn*20
        character strdev*5,bcon*10,bcode*4,lab*500
        logical toobig

        k=k*((freq/1400.d0)**(-0.7d0))

        g1=1.d0-gam
        dr1=k*(smin**g1)
        dr2=k*(smax**g1)

        call initialiseimage(image,n,m,n,m,0.d0)
        call initialiseimage(nums,n,m,n,m,0.d0)

c! area of pixel on which rms is calculated on the image must be same as
c! area going into omega_e. so take pixel area
        nsrc=k/g1*(smax**g1-smin**g1)
        write (*,*) 'one pixel per beam'
        pbtheta=3.d8/(freq*1.d6)/50.d0 ! is size of image
        area=pbtheta*pbtheta ! n pixels each pbtheta/n radian
        if (nsrc*area.gt.1.d9) then
         toobig=.true.
        else
         toobig=.false.
         totsrc=int(nsrc*area)  ! num of srcs in area
        end if
         write (*,*) nsrc,k,g1,smax**g1-smin**g1,nsrc*area
        write (*,*) ' Total number of srcs = ',totsrc
        write (*,*) '  Size of 1 pixel = ',
     /        pbtheta/(n*1.0)*180.0/3.1415*3600.0
        write (*,*) '  Area of 1 pixel = ',((pbtheta/(n*1.0))**2.d0)

        call getseed(seed)
        if (.not.toobig) then
         do 100 i=1,totsrc
          call ran1(seed,rand)
          flux=((rand*(dr2-dr1)*g1/k*k/g1+dr1))**(1.d0/g1)
          call ran1(seed,rand)
          x=int(rand*(n-1)+1)
          call ran1(seed,rand)
          y=int(rand*(n-1)+1)
          array(x,y)=array(x,y)+flux
          nums(x,y)=nums(x,y)+1.d0
          if (mod(i,totsrc/40).eq.0) write (*,'(a1,$)') '.'
100      continue
         write (*,*) 
        else
         fac1=10.d0**(int(dlog10(nsrc*area/1.d9))+1)
         write (*,*) nsrc*area,nsrc*area/1.d9,fac1
         do j=1,fac1
          do i=1,int(nsrc*area/fac1) ! shud be enuff
           call ran1(seed,rand)
           flux=((rand*(dr2-dr1)*g1/k*k/g1+dr1))**(1.d0/g1)
           call ran1(seed,rand)
           x=int(rand*(n-1)+1)
           call ran1(seed,rand)
           y=int(rand*(n-1)+1)
           array(x,y)=array(x,y)+flux
           nums(x,y)=nums(x,y)+1.d0
           if (mod(i,totsrc/40).eq.0) write (*,'(a1,$)') '.'
          end do
         end do
        end if

        call sigclip(array,n,n,1,1,256,256,std,av,5.d0)
        write (*,*) '5sig clipped std = ',std,' and mean = ',av
        write (*,*) 
        do 300 i=1,n
         do 310 j=1,n
          if (array(i,j)-av.lt.5.d0*std) then
           arr5sig(i,j)=0.d0  
          else
           arr5sig(i,j)=array(i,j)/std  ! SNR for 5-sigma clipped std
          end if
310      continue
300     continue
        
        strdev='/xs'
        lab='Image of sources'
        lab2='SNR image of > 5 sigma pixels'

c        call grey2(array,arr5sig,256,256,n,n,strdev,lab,lab2,0,1)
c        call grey2(array,nums,n,n,n,n,strdev,lab,lab2,0,1)

        f2='im1.nums'
        call writearray_bin2D(nums,n,n,n,n,f2,'mv')

        f1='im1'
        f2='imc1'
        call writearray_bin2D(array,n,n,256,256,f1,'mv')
c        sigma=8.d0
c        sig_c(1)=sigma
c        sig_c(2)=sigma
c        sig_c(3)=0.d0
c        call conv2_get_wid_fac(sig_c,n,n,s1,fac,wid)
c        bcon='periodic'
c        bcode='bare'                       ! total is preserved
c        call conv2(array,n,n,n,n,sig_c,carray,bcon,bcode,fac,s1)
c        call writearray_bin2D(carray,n,n,n,n,f2,'mv')

cc! now resample every 4 pixels of f2        
c        extn='.img'
c        call readarray_bin(n,n,arr2,n,n,f2,extn)
c
c        do 500 i=1,n,4
c         do 510 j=1,n,4
c          rarr((i+3)/4,(j+3)/4)=arr2(i,j) 
c510      continue
c500     continue
c
cc        call grey2(rarr,rarr,256,256,256,256,strdev,lab,lab2,0,1)


c! calc condon
        
        area=((pbtheta/(n*1.0))**2.d0)!*(gam-1.d0)   ! area is for one pixel 

        g1=1.d0/(gam-1.d0)
        dr1=((k*area)**g1)/((3.d0-gam)**g1)
        dr2=(3.d0-gam)*g1
        do 400 i=3,12,4  ! i=q
         write (*,*) 'q = ',i,'; sig = ',((i*1.0)**dr2)*dr1,
     /     ' D_c = ',i*((i*1.0)**dr2)*dr1,' smax = ',smax
400     continue
        s1=sqrt(k*area/(3.d0-gam))*(smax**(0.5*(3.d0-gam)))
        write (*,*)
        write (*,*) ' For sigma=calc, q = ',(std/dr1)**(1.d0/dr2)
        write (*,*) ' For D_c=Smax, sigma= ',s1,' ; q = ',smax/s1
        write (*,*)

        do 430 i=1,10
         onesrc=(-1.d0/(i*5.0)*(1.d0-gam)/(area*k))**(1.d0/(1.d0-gam))
         s1=sqrt(k*area/(3.d0-gam))*(onesrc**(0.5*(3.d0-gam)))
         write (*,*) ' For N(>S)=',1.0/(5.0*i),' sig= ',s1,
     /               '; q = ',onesrc/s1
430     continue
        
        write (*,*)
        
        

        return
        end


