c! create images which are basically gaussian with some added sidelobes using noise.f
c! and then decompose by shapelets and get statistics of residuals and so on.
c!
c! Not broken up into simulating imaging and analysis seperately, to save space of
c! storing all the images.


        implicit none
        integer n,m,nimage
        character sav*1
        
        write (*,*) 
        write (*,*) '     This program creates many random images, and '
        write (*,*) '     analyses them by various methods.'
        write (*,*) 
        write (*,*) '     ###  Very slow as I convolve in image plane'
        write (*,*) '     ###  I should change that to FFT soon .... '
        write (*,*) 
        write (*,'(a32,$)') '   Enter size of images (n,m) : '
        read (*,*) n,m
        write (*,'(a36,$)') '   Number of images to be created : '
        read (*,*) nimage
334     continue
        write (*,'(a27,$)') '   Save all images (y/n) : '
        read (*,*) sav
        if (sav.ne.'y'.and.sav.ne.'n') goto 334
        
        call produceim_strongsrc(n,m,nimage,sav)

        end

        subroutine produceim_strongsrc(n,m,nimage,sav)
        implicit none
        include "constants.inc"
        integer n,m,nimage,seed,i,nhc,nmax
        real*8 rsnr(2),rfw(2),rfracfw(2),rfracsnr(2),rnrest(2)
        real*8 snr,fw,fracfw,fracsnr,nrest,dumr
        real*8 image(n,m),mask(n,m),totg1(nimage),rmsg1(nimage)
        real*8 totsh(nimage),rmssh(nimage),q
        real*8 maskarea,total(nimage),rms(nimage),av,nsig
        character sav*1,fn*40,fname*40

        write (*,*) 
        write (*,*) '  Will create strongly peaked source and'
        write (*,*) '  parameters in file have uniform distribution'
        write (*,*) 
        write (*,*) '   Not yet done proper nmax, and elliptical gauss '
        write (*,*) '   and diff beta for shapelets'
        write (*,*) 

c! This is to get parameters to create random image with a single strong peak 
c! min and max for --  snr, fw of gaus kernel, fraction of snr extra to spread out in
c! fracfw area using nrest pixels.

        nhc=31
        nmax=8

        call get_rndmim_para(rsnr,rfw,rfracfw,rfracsnr,rnrest,n,m,fn)
        
        call getseed(seed)
        do 100 i=1,nimage
         call getrand(seed,rsnr,snr)
         call getrand(seed,rfw,fw)
         call getrand(seed,rfracfw,fracfw)
         call getrand(seed,rfracsnr,fracsnr)
         call getrand(seed,rnrest,nrest)

         call makeim_strongsrc(image,n,m,snr,fw,fracfw,fracsnr,  ! image has input image
     /        nrest,seed)                         

         if (sav.eq.'y') then
          call getsavedimname(fn,i,nimage,fname)
          call writearray_bin(image,n,m,n,m,fname)
         end if

c! now fit, and get total area and residual errors by foll methods
c! plot recovered area, residual noise vs snr, fracsnr, fracfw 
c! we have formisland_basic and _rank.
         if (fw.ge.2.35d0) then 
          nsig=3.d0
         else
          nsig=2.d0
         end if
         
         call formisland_rank(image,n,m,n,m,mask,nsig,sav,fname)   ! out to n sigma,1=good,0=bad
         call get_totrms_proper(image,mask,n,m,n,m,total(i),rms(i))  ! do 1 gaus analysis
         call get_totrms_gaus1(image,mask,n,m,n,m,totg1(i),rmsg1(i),
     /                 sav,fname,q)       ! do 1 gaus analysis
         call get_totrms_shape(image,mask,n,m,n,m,totsh(i),rmssh(i),
     /                 sav,fname,nhc,nmax)       ! do shape analysis
c         call get_totrms_gaus2(image,mask,n,m,n,m,totg2(i),rmsg2(i))  ! do 2 gaus analysis

         
       write (*,'(a1,i3,1x,9(f7.3,1x))') 
     /     'Z',i,totg1(i)/total(i),rmsg1(i)/rms(i),
c     /      totsh(i)/total(i),rmssh(i)/rms(i),
     /      snr,fw,fracfw,fracsnr,q
       write (*,*)

100     continue

        return
        end


c!        ---------------------    SUBROUTINES   ------------------------- 
        
c! read in the parameter file.
        subroutine get_rndmim_para(snr,fw,fracfw,fracsnr,nrest,n,m,fn)
        implicit none
        real*8 snr(2),fw(2),fracfw(2),fracsnr(2),nrest(2)
        integer nchar,n,m,ok
        character fn*40

333     continue
        write (*,'(1x,a31,$)') '  Config file for parameters : '
        read (*,*) fn
        open(unit=21,file=fn(1:nchar(fn)),status='old')
        read (21,*) snr(1),snr(2)
        read (21,*) fw(1),fw(2)
        read (21,*) fracfw(1),fracfw(2)
        read (21,*) fracsnr(1),fracsnr(2)
        read (21,*) nrest(1),nrest(2)
        close(21)

        ok=1
        if (snr(2).le.snr(1).or.fw(2).le.fw(1).or.fracfw(2).le.
     /      fracfw(1).or.fracsnr(2).le.fracsnr(1).or.nrest(2).le.
     /      nrest(1)) ok=0
        if (fracsnr(2).gt.4.d0) ok=0
        if (fracfw(2).gt.5.d0) ok=0
        if (snr(1).lt.3.d0) ok=0
        if (fw(2).ge.0.4d0*min(n,m)) ok=0
        
        if (ok.eq.0) then
         write (*,*) '  File values not okay'
         goto 333
        end if
        write (*,*) 

        return
        end
        
c! to return randum num in a range
        subroutine getrand(seed,rnge,rand)
        implicit none
        integer seed
        real*8 rnge(2),rand

        call ran1(seed,rand)
        rand=(rnge(2)-rnge(1))*rand+rnge(1)
        
        return
        end


        subroutine getsavedimname(fn,i,nimage,fname)
        implicit none
        integer i,nimage,digit,ndigit,num,j,rem,nchar
        character fn*40,fname*40,getchar,str*10

        str=' '
        ndigit=int(log10(nimage*1.0))+1
        digit=int(log10(i*1.0))+1
        num=i
        do 100 j=1,digit
         rem=num-int(num*0.1)*10
         str(ndigit-j+1:ndigit-j+1)=getchar(rem)
         num=int(num*0.1)
100     continue
        do 110 j=1,ndigit-digit
         str(j:j)='0'
110     continue
        fname=fn
        fname(nchar(fn)+1:nchar(fn)+1)='.'
        fname(nchar(fn)+2:nchar(fn)+2+ndigit)=str(1:ndigit)

        return
        end

