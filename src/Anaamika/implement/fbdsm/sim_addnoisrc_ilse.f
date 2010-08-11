c! read from id.mos, get mosaic centres, cutout the noise and src images,
c! do primary beam for src image, add to noise, write to fits.
c! modify for ilse , no convolution, no noise.

        subroutine sim_addnoisrc_ilse(fn,imsize,nchan,freq,
     /           synbeam,crval1,
     /           crval2,crpix1,crpix2,cdelt1,cdelt2,bw,scratch,fitsdir)
        implicit none
        character fn*500,f1*500,scratch*500,fitsdir*500
        integer nchar,nmosx,nmosy,ipt,ptsizex,ptsizey,nchan
        integer blc(3),trc(3),imsize,nptx,npty,ifac,round
        integer maxpt
        parameter (maxpt=20)
        integer xptcen(maxpt),yptcen(maxpt)
        real*8 freq,synbeam,bw
        real*8 cdelt1,cdelt2
        real*8 crval1,crval2,crpix1,crpix2

        f1=scratch(1:nchar(scratch))//fn(1:nchar(fn))//'.mos'
        open(unit=23,file=f1,status='unknown')
        read (23,*) nptx,npty
        if (nptx*npty.gt.maxpt) write (*,*) '   #### INCREASE maxpt'
        do ipt=1,nptx*npty
         read (23,*) blc(1),blc(2),trc(1),trc(2)
         xptcen(ipt)=(blc(1)+trc(1))/2
         yptcen(ipt)=(blc(2)+trc(2))/2
        end do
        write (*,*) '  Attenuating for primary beam ... '
        call primarybeam3D_many(fn,imsize,imsize,nchan,cdelt1,freq,
     /       50.d0,nptx*npty,xptcen,yptcen,maxpt,scratch)

        read (23,*) nmosx,nmosy
        blc(3)=1
        trc(3)=nchan
        write (*,*) '  Constructing images for ... '
        do 100 ifac=1,nmosx*nmosy     ! for each facet to cut out
         read (23,*) blc(1),blc(2),trc(1),trc(2)
         ptsizex=trc(1)-blc(1)+1
         ptsizey=trc(2)-blc(2)+1
         write (*,'(a19,i3)') '   pointing number ',ifac
         call sub_sim_addsrcnoi_ilse(fn,imsize,nchan,blc,trc,ptsizex,
     /      ptsizey,ifac,nmosx*nmosy,freq,cdelt1,cdelt2,synbeam,crval1,
     /         crval2,crpix1,crpix2,bw,fitsdir)  ! does the work
100     continue
        close(23)

        return
        end


        subroutine sub_sim_addsrcnoi_ilse(fn,imsize,nchan,blc,trc,
     /             ptsizex,
     /             ptsizey,ipt,npt,freq,cdelt1,cdelt2,synbeam,crval1,
     /             crval2,crpix1a,crpix2a,bw,fitsdir)
        implicit none
        include "constants.inc"
        character fn*500,f1*500
        integer nchar,ptsizex,ptsizey,nchan,ipt
        integer blc(3),trc(3),i,j,k,imsize,xc,yc,npt
        real*8 sub_noi(ptsizex,ptsizey,nchan)
        real*8 sub_im(ptsizex,ptsizey,nchan)
        real*8 freq,synbeam,bw
        real*8 cdelt1,cdelt2,crval3,crpix3
        real*8 crval1,crval2,crpix1a,crpix2a
        real*8 crpix1,crpix2
        character telescop*8,receiver*8,object*8,observer*8
        character ctype1*8,ctype2*8,ctype3*8,f2*5,filename*500
        character fitsdir*500
        real*8 epoch,bmaj,bmin,bpa,cdelt3
        integer fproduct

        f1=fn(1:nchar(fn))//'.src3D.pbeam'
        call subim3D(f1,imsize,imsize,nchan,sub_im,ptsizex,
     /               ptsizey,nchan,blc,trc)

c        f1=fn(1:nchar(fn))//'.noi'
c        call subim3D(f1,imsize,imsize,nchan,sub_noi,ptsizex,
c     /               ptsizey,nchan,blc,trc)

        telescop='LOFAR' 
        if (freq.le.100.d6) then
         receiver='Lo-Band'
        else
         receiver='Hi-Band'
        end if
        observer='Niruj'
        ctype1='RA---SIN'
        ctype2='DEC--SIN'
        ctype3='FREQ'
        call getextnnum(ipt,npt,f2)
        object=fn(1:nchar(fn))
        filename=fn(1:nchar(fn))//'.'//f2(1:nchar(f2))

        bmaj=synbeam*rad
        bmin=synbeam*rad
        bpa=0.d0
        crval3=freq
        crpix3=nchan/2
        cdelt3=bw/nchan
        epoch=2006.2d0

        crpix1=crpix1a-blc(1)
        crpix2=crpix2a-blc(2)
      
        call writefits3D(sub_im,ptsizex,ptsizey,nchan,telescop,filename,
     /    receiver,object,observer,epoch,fproduct,bmaj,bmin,bpa,ctype1,
     /    ctype2,ctype3,crval1,crval2,crval3,crpix1,crpix2,crpix3,
     /    cdelt1,cdelt2,cdelt3,fitsdir)

        return
        end



        subroutine getextnnum(ipt,npt,f2)
        implicit none
        integer ipt,npt,ndigit,digit,j,rem,num
        character f2*5,fn*500,fname*500,getchar

        f2=' '
        ndigit=int(log10(npt*1.0))+1
        digit=int(log10(ipt*1.0))+1
        num=ipt
        do 100 j=1,digit
         rem=num-int(num*0.1)*10
         f2(ndigit-j+1:ndigit-j+1)=getchar(rem)
         num=int(num*0.1)
100     continue
        do 110 j=1,ndigit-digit
         f2(j:j)='0'
110     continue
        
        return
        end


