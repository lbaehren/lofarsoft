c! will use moment analysis of some sort to calc parameters of extended sources (gaultosrc)
c! this is to test how good it is as fn of SNR.
c! this creates sources, convolves with different psfs and puts noise, and makes the
c! source list. To test BDSM with, exactly. copy most stuff from simulation.f
c! Have range of SNRs and for each SNR, have same range in bmaj/bmin.
c! Make sure image has 30 beams for the one source (for calc of rms).
c! But hardcode that bmin is 4 pixels, take max bmaj, so image size is, say, 10(max(bmaj))

        subroutine testmomentanal(filename,scratch,seed)
        implicit none
        real*8 snrmin,snrmax,dumr,armin,armax
        integer arnum,snrnum,seed,imsize,round
        character scratch*500,filename*500

        write (*,*)
        write (*,*) '  Tests moment analysis of gaussian images'
        write (*,*)

334     write (*,'(a,$)') '  Range of aspect ratio (min, max) : '
        read (*,*) armin,armax
        dumr=min(armin,armax)
        armax=max(armin,armax)
        armin=dumr
        if (armin.lt.1.d0.or.armax.lt.1.d0) goto 334

        write (*,'(a,$)') '  Range of SNR (min, max), number : '
        read (*,*) snrmin,snrmax
        dumr=min(snrmin,snrmax)
        snrmax=max(snrmin,snrmax)
        snrmin=dumr

333     write (*,'(a,$)') '  Number of aspect ratios; SNRs : '
        read (*,*) arnum,snrnum
c        if (arnum.lt.2.or.snrnum.lt.2) goto 333

        imsize=round(4.d0*armax*10.d0)
        call sub_testmomentanal(armin,armax,snrmin,snrmax,
     /       arnum,snrnum,seed,imsize)

        return
        end
c!
c!      ==========
c!
        subroutine sub_testmomentanal(armin,armax,snrmin,snrmax,
     /       arnum,snrnum,seed,imsize)
        implicit none
        include "constants.inc"
        integer arnum,snrnum,seed,imsize,i,isnr,iar,round,i1,j1
        real*8 snrmin,snrmax,dumr,armin,armax,rand
        real*8 snr(snrnum),ar(arnum),bmaj,bmin,bpa,a4,a5,a6
        real*8 noise(imsize,imsize),src(imsize,imsize),mompara(6)
        real*8 image(imsize,imsize),x0,y0,sig_c(3),s1,fac,wid
        real*8 cnoise(imsize,imsize),bmar_p
        character scratch*500,bcon*10,bcode*4,filename*500

c! first calc SNRs and aspect ratios.
        dumr=(snrmax/snrmin)**(1.d0/(snrnum-1.d0))
        snr(1)=snrmin
        do i=2,snrnum
         snr(i)=snr(i-1)*dumr                    ! flux in units of snr
        end do
        do i=1,arnum
         call ran1(seed,rand)
         ar(i)=rand*(armax-armin)+armin          ! bmaj/bmin
        end do

        do isnr=1,snrnum                         ! for each snr ...
         do iar=1,arnum
c         write (*,*) isnr,iar
          call ran1(seed,rand)
          bpa=rand*180.d0 
          bmin=4.d0
          bmaj=bmin*ar(iar)
          a4=bmaj/fwsig       ! pixels sigma
          a5=bmin/fwsig       ! pixels sigma
          a6=(bpa-90.d0)/rad  ! rad
          bmar_p=2.d0*pi*a4*a5
c! get central position
          call ran1(seed,rand)
          x0=round(imsize/3.d0)+rand*4.d0-2.d0
          call ran1(seed,rand)
          y0=round(imsize/3.d0)+rand*4.d0-2.d0
c         write (*,*) ' create src '
c! create source map
          do i1=1,imsize
           do j1=1,imsize
            src(i1,j1)=snr(isnr)*dexp(-0.5d0*(
     /       (((i1-x0)*dcos(a6)+(j1-y0)*dsin(a6))/a4)**2.d0+
     /       (((j1-y0)*dcos(a6)-(i1-x0)*dsin(a6))/a5)**2.d0))
           end do
          end do
c! create raw noise map
c         write (*,*) ' create noi '
          sig_c(1)=a4
          sig_c(2)=a5
          sig_c(3)=0.d0
          call conv2_get_wid_fac(sig_c,imsize,imsize,s1,fac,wid)
          call xcr8noisemap(imsize,imsize,imsize,imsize,
     /         1.d0/fac,noise,seed)
c! convolve noise map and then add to source
          bcon='periodic'
          bcode='bare'                       ! total is preserved
          sig_c(3)=a6*rad
c         write (*,*) ' conv noi '
          call conv2(noise,imsize,imsize,imsize,imsize,sig_c,
     /        cnoise,bcon,bcode,fac,s1)
c         write (*,*) ' add src and noi '
          do i1=1,imsize
           do j1=1,imsize
            image(i1,j1)=cnoise(i1,j1)+src(i1,j1)
           end do
          end do
          
         filename='dum.add'
c         call writearray_bin2D(image,imsize,imsize,imsize,imsize,
c     /       filename,'mv')

c! Now estimate parameters
          call callmomanal(image,imsize,imsize,mompara,bmar_p)
          write (*,'(12(f11.5,1x))') 
     /      snr(isnr),x0,y0,bmaj,bmin,bpa,mompara

         end do
        end do 

        return
        end
c!
c!      ==========
c!
        subroutine callmomanal(image,n,m,mompara,bmar_p)
        implicit none
        integer n,m,mask(n,m),i,j,im,jm,ndigit,fac
        integer now,next,nrank(100),co_now(n*m)
        integer xsurr(8),ysurr(8),co_next(n*m),i8
        real*8 image(n,m),mompara(6),std,av
        real*8 iimage(n,m),bmar_p,dumr
        character filename*500
        logical pixinimage

        dumr=image(1,1) 
        do i=1,n
         do j=1,m
          if (image(i,j).gt.dumr) then
           dumr=image(i,j)
           im=i
           jm=j
          end if
         end do
        end do

c! prepare mask
        call sigclip(image,n,m,1,1,n,m,std,av,3.d0)
        ndigit=int(log10(max(n,m)*1.0))+1      ! number of digits in max(n,m)
        fac=int(10**ndigit)                    ! for compressing coords
        call initialisemask(mask,n,m,n,m,0)
        if (mask(im,jm).eq.0) then
         now=1
         nrank(now)=1          ! number of pix with mask=1 is 1
         mask(im,jm)=now
         co_now(1)=jm*fac+im

333      continue
         next=now+1
         nrank(next)=0
         do 100 i=1,nrank(now)
          call getsurr_8pix(co_now(i),xsurr,ysurr,fac)   ! x,ysurr has surrounding pix
          do 110 i8=1,8
           if (pixinimage(xsurr(i8),ysurr(i8),1,n,1,m)) then
            if (image(xsurr(i8),ysurr(i8))-av.ge.2.d0*std) then ! has emission
             if (mask(xsurr(i8),ysurr(i8)).eq.0) then     ! new good pixel
              mask(xsurr(i8),ysurr(i8))=next
              nrank(next)=nrank(next)+1 
              co_next(nrank(next))=ysurr(i8)*fac+xsurr(i8)
             end if ! new good pixel
            end if ! is pixel is above threshold
           end if ! if pixel is inside image
110       continue
100      continue
         now=next   ! make old=new
         do 120 i=1,nrank(next)
          co_now(i)=co_next(i)
120      continue
         if (nrank(now).gt.0) goto 333
        end if  ! mask of xpt,ypt ne 0

        do i=1,n
         do j=1,m
          if (mask(i,j).gt.0) mask(i,j)=1
          iimage(i,j)=image(i,j)*mask(i,j) !mask(i,j)
         end do
        end do
        filename='dum.mask'
c         call writearray_bin2D(iimage,n,m,n,m,filename,'mv')

        call momanal(image,n,m,mask,mompara,bmar_p)
         
        return
        end
c!
c!      ==========
c!
        subroutine momanal(image,n,m,mask,mompara,bmar_p)
        implicit none
        include "constants.inc"
        integer n,m,mask(n,m),i,j
        real*8 image(n,m),mompara(6),mom1(2),mom2(2),m11,tot,bmar_p
        real*8 dumr

        mom1(1)=0.d0
        mom1(2)=0.d0
        mom2(1)=0.d0
        mom2(2)=0.d0
        m11=0.d0
        tot=0.d0
        do i=1,n
         do j=1,m
          if (mask(i,j).eq.1) then
           tot=tot+image(i,j)
           mom1(1)=mom1(1)+i*image(i,j)
           mom1(2)=mom1(2)+j*image(i,j)
          end if
         end do
        end do
        mompara(1)=tot/bmar_p
        mompara(2)=mom1(1)/tot
        mompara(3)=mom1(2)/tot
        do i=1,n
         do j=1,m
          if (mask(i,j).eq.1) then
           mom2(1)=mom2(1)+(i-mompara(2))*(i-mompara(2))*image(i,j)
           mom2(2)=mom2(2)+(j-mompara(3))*(j-mompara(3))*image(i,j)
           m11=m11+(i-mompara(2))*(j-mompara(3))*image(i,j)
          end if
         end do
        end do
        mompara(4)=sqrt((mom2(1)+mom2(2)+sqrt((mom2(1)-mom2(2))*
     /    (mom2(1)-mom2(2))+4.d0*m11*m11))/(2.0d0*tot))*fwsig
        mompara(5)=sqrt((mom2(1)+mom2(2)-sqrt((mom2(1)-mom2(2))*
     /    (mom2(1)-mom2(2))+4.d0*m11*m11))/(2.0d0*tot))*fwsig
        dumr=atan(abs(2.d0*m11/(mom2(1)-mom2(2))))
        call atanproper(dumr,2.d0*m11,mom2(1)-mom2(2))
        mompara(6)=0.5d0*dumr*rad-90.d0
        if (mompara(6).lt.0.d0) mompara(6)=mompara(6)+180.d0

        return
        end





