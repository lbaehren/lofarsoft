c! to make a random image for strong pt src

        subroutine makeim_strongsrc(image1,n,m,snr,fw,rfracfw,
     /             fwzero,fracsnr,nrest,seed)
        implicit none
        include "constants.inc"
        integer n,m,xc0,yc0,round,i,xc,yc,seed,j
        real*8 image(n,m),snr,fw(3),rfracfw(2),fracsnr,nrest
        real*8 newsnr,dumr(2),rms,rand,sigma(3)
        real*8 image1(n,m),fac,fwzero,rand1,unity(2)
        character bcon*10,bcode*4,f1*500,rcode*2
        real*8 std1,std2,av,s1

        rms=1.0d0 ! 1 Jy
        data unity/0.d0,1.d0/
        
        dumr(1)=0.25*min(n,m)
        dumr(2)=0.75*min(n,m)  ! this sets the limits for centre
        call getrand(seed,dumr,rand)
        xc0=round(rand)
        call getrand(seed,dumr,rand)
        yc0=round(rand)
        
c! create noise image, put in main source
        bcode='none'
        sigma(1)=fw(1)/fwsig ! fwhm --> sigma
        sigma(2)=fw(2)/fwsig ! fwhm --> sigma
        sigma(3)=fw(3) ! fwhm --> sigma
        call conv2(image,n,m,n,m,sigma,image1,bcon,bcode,fac,s1) ! image1 is dummy
        call xcr8noisemap(n,m,n,m,rms/fac,image,seed)

        image(xc0,yc0)=snr*rms*s1  ! so that new snr is correct

c! now put in the extra flux
        do 100 i=1,round(nrest)
335      continue
         call getrand(seed,rfracfw,rand)
         call getrand(seed,unity,rand1)
         rand=(rand+fwzero)*sqrt(fw(1)*fw(2))
         if (rand1.lt.0.5d0) rand=-rand
         xc=xc0+round(rand)
         call getrand(seed,rfracfw,rand)
         call getrand(seed,unity,rand1)
         rand=(rand+fwzero)*sqrt(fw(1)*fw(2))
         if (rand1.lt.0.5d0) rand=-rand
         yc=yc0+round(rand)
         if (xc.lt.1.or.xc.gt.n.or.yc.lt.1.or.yc.gt.m) goto 335
c         image(xc,yc)=image(xc,yc)+snr*rms*s1*fracsnr/nrest
100     continue
c         rcode='aq'
c         f1='b4c'
c         call writearray_bin2D(image,n,m,n,m,f1,rcode)

c! now convolve with gaussian
        bcon='periodic'
        bcode='bare'
        call conv2(image,n,m,n,m,sigma,image1,bcon,bcode,fac,s1)
c         f1='a5c'
c         call writearray_bin2D(image1,n,m,n,m,f1,rcode)
        
        return
        end


