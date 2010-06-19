c! create images which are basically gaussian with some added sidelobes using noise.f
c! and then decompose by shapelets and get statistics of residuals and so on.
c!
c! Not broken up into simulating imaging and analysis seperately, to save space of
c! storing all the images.


        subroutine simul_shapelets(scratch,srldir,runcode)
        implicit none
        integer n,m,nimage
        character sav*1,scratch*500,srldir*500,runcode*2
        
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

        return
        end

        subroutine produceim_strongsrc(n,m,nimage,sav)
        implicit none
        include "constants.inc"
        include "trhermcoef.inc"
        character sav*1,fn*500,fname*500,runcode*2
        integer n,m,nimage,seed,i,nmax,mask(n,m)
        real*8 rsnr(2),rfw(2),rfracfw(2),rfracsnr(2),rnrest(2)
        real*8 snr,fw(3),fracfw,fracsnr,nrest,dumr,fwzero,pa(2)
        real*8 image(n,m),totg1(nimage),rmsg1(nimage)
        real*8 totsh(nimage),rmssh(nimage),q
        real*8 maskarea,total(nimage),rms(nimage),av,nsig
        
        data pa/0.d0,180.d0/

        write (*,*) 
        write (*,*) '  Will create strongly peaked source and'
        write (*,*) '  parameters in file have uniform distribution'
        write (*,*) 
        write (*,*) '   Not yet done proper nmax, and elliptical gauss '
        write (*,*) '   and diff beta for shapelets'
        write (*,*) 

        runcode='aq'
c! This is to get parameters to create random image with a single strong peak 
c! min and max for --  snr, fw of gaus kernel, fraction of snr extra to spread out in
c! fracfw area using nrest pixels.

        nmax=8
        call get_rndmim_para(rsnr,rfw,rfracfw,fwzero,rfracsnr,rnrest,
     /       n,m,fn)
        
        call getseed(seed)
        do 100 i=1,nimage
         call getrand(seed,rsnr,snr)
         call getrand(seed,rfw,fw(1))
         call getrand(seed,rfw,fw(2))
         call getrand(seed,pa,fw(3))
         call getrand(seed,rfracsnr,fracsnr)
         call getrand(seed,rnrest,nrest)
         if (fw(1).lt.fw(2)) call xchangr8(fw(1),fw(2))

         call makeim_strongsrc(image,n,m,snr,fw,rfracfw,fwzero,fracsnr, ! image has input image
     /        nrest,seed)                         

         if (sav.eq.'y') then
          call getsavedimname(fn,i,nimage,fname)
          call writearray_bin2D(image,n,m,n,m,fname,runcode)
         end if

c! now fit, and get total area and residual errors by foll methods
c! plot recovered area, residual noise vs snr, fracsnr, fracfw 
c! we have formisland_basic and _rank.
         if (fw(2).ge.2.35d0) then 
          nsig=3.d0
         else
          nsig=2.d0
         end if
          nsig=1.5d0
         
         call formisland_rank(image,n,m,n,m,mask,nsig,sav,fname)   ! out to n sigma,1=good,0=bad
         write (*,*) 'islands done'
         call get_totrms_proper(image,mask,n,m,n,m,fw,total(i),rms(i))  !  do arrstatmask and sigclip
         write (*,*) total(i),rms(i)
c         call get_totrms_gaus1(image,mask,n,m,n,m,totg1(i),rmsg1(i),
c     /                 sav,fname,q)       ! do 1 gaus analysis. returns total and residual rms
c         call get_totrms_shape(image,mask,n,m,n,m,totsh(i),rmssh(i),
c     /                 sav,fname,nhc,hc,nmax)       ! do shape analysis
         
       write (*,'(a1,i3,1x,13(e12.4,1x))') 
     /     'Z',i,totg1(i),total(i),rmsg1(i),rms(i),
     /      totsh(i),total(i),rmssh(i),rms(i)
     /      ,snr

100     continue

        return
        end


c!        ---------------------    SUBROUTINES   ------------------------- 
        
c! read in the parameter file.
        subroutine get_rndmim_para(snr,fw,fracfw,fwzero,fracsnr,nrest,
     /             n,m,fn)
        implicit none
        real*8 snr(2),fw(2),fracfw(2),fracsnr(2),nrest(2),fwzero
        integer nchar,n,m,ok
        character fn*500

        write (*,'(1x,a31,$)') '  Config file for parameters : '
c        read (*,*) fn
        fn='shapepara'
        open(unit=21,file=fn(1:nchar(fn)),status='old')
        read (21,*) snr(1),snr(2)
        read (21,*) fw(1),fw(2)
        read (21,*) fracfw(1),fracfw(2)
        read (21,*) fwzero
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
        fwzero=abs(fwzero)
        
        if (ok.eq.0) then
         write (*,*) '  File values not okay'
         stop
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
        character fn*500,fname*500,getchar,str*10

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

