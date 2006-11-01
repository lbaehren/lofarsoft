c! this is to test confusion noise formalism of condon thro simulating images.
c! rather, to test my understanding of it.
c! g77 testconfusionnoise.f clickop.f sigclip.f ran1.f arrstat.f sub_prog.f -o 
c! testconfusionnoise -L/usr/lib -L/usr/X11R6/lib -lpng -lX11 -L$PGPLOT_DIR -lpgplot


        implicit none
        real*8 k,gam,smin,smax,rand,g1,dr1,dr2,flux
        real*8 array(1024,1024),std,av,arr5sig(1024,1024),freq
        real*8 area,pbtheta,th,nsrc,s1,onesrc
        integer totsrc,seed,x,y,i,j,n,nchar
        real*8 nums(1024,1024),sigma,arr2(1024,1024),rarr(256,256)
        character strdev*5,lab*500,lab2*500,f1*500,f2*500,extn*10
        
        k=10.d0
        gam=2.2d0  ! k* s_jy^-gam is diff num cts at 1.4 GHz
        g1=1.d0-gam
        smin=1.d-7 ! Jy
        smax=1.d-3  ! Jy
        dr1=smin**g1
        dr2=smax**g1

        n=1024
        do 200 i=1,n
         do 210 j=1,n
          array(i,j)=0.d0
          nums(i,j)=0.d0
210      continue
200     continue

c! area of pixel on which rms is calculated on the image must be same as
c! area going into omega_e. so take pixel area
        nsrc=k/g1*(smax**g1-smin**g1)
        write (*,*) ' freq : 30, 75, 120, 200 MHz'
        read (*,*) freq
        write (*,*) 'one pixel per beam'
        pbtheta=3.d8/(freq*1.d6)/50.d0 ! is size of image
        area=pbtheta*pbtheta ! n pixels each pbtheta/n radian
        totsrc=int(nsrc*area)  ! num of srcs in area
        write (*,*) ' Total number of srcs = ',totsrc
        write (*,*) '  Size of 1 pixel = ',
     /        pbtheta/(n*1.0)*180.0/3.1415*3600.0
        write (*,*) '  Area of 1 pixel = ',((pbtheta/(n*1.0))**2.d0)

        call getseed(seed)
        do 100 i=1,totsrc
         call ran1(seed,rand)
         flux=((rand*(dr2-dr1)*g1/k+dr1))**(1.d0/g1)
         call ran1(seed,rand)
         x=int(rand*n+1)
         call ran1(seed,rand)
         y=int(rand*n+1)
         array(x,y)=array(x,y)+flux
         nums(x,y)=nums(x,y)+1.d0
         if (mod(i,totsrc/40).eq.0) write (*,'(a1,$)') '.'
100     continue
        write (*,*) 

        call sigclip(array,256,256,1,1,n,n,std,av,5)
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
        call grey2(array,nums,1024,1024,n,n,strdev,lab,lab2,0,1)
        pause

        f1='im'
        f2='imc'
        call writearray_bin(array,256,256,n,n,f1,'mv')
        sigma=8.d0
        call convolveimage(f1,f2,sigma)

c! now resample every 4 pixels of f2        
        extn='.img'
        call readarray_bin(n,n,arr2,1024,1024,f2,extn)

        do 500 i=1,1024,4
         do 510 j=1,1024,4
          rarr((i+3)/4,(j+3)/4)=arr2(i,j) 
510      continue
500     continue

        call grey2(rarr,rarr,256,256,256,256,strdev,lab,lab2,0,1)


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


