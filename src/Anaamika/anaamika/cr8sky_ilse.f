c! Creates the sky with sources.
c! adding noise seperately outside tho it will waste time, for flexibility.
c! Break up source counts into 5 bins, fit power law to each, invert, calc analytically fn of rand.

c! modify for ilse so u dont convolve with gaussian -- she wants just delta fn sources.
c! which means all sources will be at centres of pixels of course.

        subroutine cr8sky_ilse(freq,bw,synbeam,cdelt1,imsize,
     /    bmsampl,xsize,nchan,sens,cn1,sarr,gam1,nbin,seed,fn,
     /    std,srldir,beamvary,bmvaryf,bmvaryn)
        implicit none
        include "constants.inc"
        integer xsize
        integer nbin,i,j,imsize,seed,xc,yc
        integer xca,yca,round,i1,j1,nchar,nchan,ich
        real*8 xc1,yc1,xc2,yc2
        real*8 cn1(500),sarr(500),gam1(500),dumr1,dumr2
        real*8 nsrc(500/nbin),low(500/nbin),high(500/nbin)
        real*8 freq,synbeam,cdelt1,sens,rand
        real*8 image(imsize,imsize),sizeas,flux
        real*8 xposn,yposn,bmsampl,bw,sizepix
        real*8 fluxim(xsize,xsize),std,av
        real*8 cube(imsize,imsize,nchan),spin,f1
        real*8 fchan,fac1,bmaj,bmin,bpa,bmajpix
        character filen*500,fn*500,srldir*500,beamvary
        integer bmvaryn
        real*8 bmvaryf(bmvaryn),gasdev

        write (*,*) '  Initialising the image ... '
        call initialiseimage(image,imsize,imsize,imsize,imsize,0.d0)

        open(unit=21,file=srldir(1:nchar(srldir))//
     /       fn(1:nchar(fn))//'.srclist',status='unknown')

c! get high, low and nsrc in each of the (500/nbin) bins
        call getnsrcbin(nbin,sarr,cn1,gam1,low,high,nsrc)

        write (*,'(a51,$)') 
     /       '   Creating sources, convolve, adding to image ... '
        do 100 i=1,500/nbin     !  the bins
         do 110 j=1,int(nsrc(i))     ! put in each source in that bin
c! get random flux, position and size
          call getrandflux(low(i),high(i),cn1(i),gam1(i),rand,flux,seed)! is correct
          call getrandposn(imsize,imsize,xposn,yposn,seed)
          call getrandsize(flux,freq,sizeas,seed)   ! size in asec
          sizepix=sizeas/cdelt1
          bmin=bmsampl*cdelt1
          bmaj=sqrt((bmsampl*cdelt1)**2.d0+sizeas*sizeas)  ! hack
          bmajpix=bmaj/cdelt1
          if (sizepix.gt.0.2*bmsampl.and.flux.ge.3.d0*sens) then
           call ran1(seed,rand)
           bpa=rand*180.d0   ! 0-180 deg
          else
           bpa=0.d0
          end if

c! get the image, of pt src of flux, convled with fw=bmsampl gauss at x,y of size simsize
c! round(xposn,yposn) == (xc,yc)
         if (flux.le.2.d0*sens.or.bmaj/bmin.le.1.1d0) then
          image(round(xposn),round(yposn))=image(round(xposn),
     /          round(yposn))+flux
        else
         xc1=xposn+sizepix/2*dcos(bpa/rad)
         yc1=yposn+sizepix/2*dsin(bpa/rad)
         xc2=xposn-sizepix/2*dcos(bpa/rad)
         yc2=yposn-sizepix/2*dsin(bpa/rad)
333      continue
         rand=gasdev(seed)*0.2+0.5  ! is a hack
         if (rand.le.0.1d0.or.rand.ge.0.9d0) goto 333
         image(round(xc1),round(yc1))=image(round(xc1),round(yc1))
     /         +flux*rand
         image(round(xc2),round(yc2))=image(round(xc2),round(yc2))
     /         +flux*(1.d0-rand)
         end if

c! write source list
          if (flux.ge.5.d0*sens) then
           if (flux.le.2.d0*sens.or.bmaj/bmin.le.1.1d0) then
            call writesrclist(21,round(xposn),round(yposn),flux,
     /           0.d0,0.d0,0.d0)
           else
            call writesrclist(21,round(xc1),round(yc1),rand*flux,
     /           0.d0,0.d0,0.d0)
            call writesrclist(21,round(xc2),round(yc2),rand*flux,
     /           0.d0,0.d0,0.d0)
           end if
          end if
 
          if (nsrc(i).gt.5.d5.and.mod(j,1000000).eq.0) 
     /       write (*,'(a1,$)') '.'

110      continue
100     continue
        close(21)   ! srclist
        write (*,*)

c! calculate 3-sigma clipped noise and send back
        call sigclip(image,imsize,imsize,1,1,imsize,imsize,std,av,3.d0)

        filen=fn(1:nchar(fn))//'.src'
        call writearray_bin2D(image,imsize,imsize,imsize,imsize,
     /       filen,'mv')

c! Use spectral index to make a source cube.
        f1=freq-bw/2.d0+bw/nchan/2.d0
        do i=1,imsize
         do j=1,imsize
          call ran1(seed,rand)
          spin=rand*0.2d0-0.8d0   ! spec index with mean -0.8 and disp 0.2
          do ich=1,nchan
           fchan=f1+(ich-1)*bw/nchan
           fac1=(fchan/freq)**spin
           cube(i,j,ich)=image(i,j)*fac1
          end do
         end do
        end do
        
        filen=fn(1:nchar(fn))//'.src3D'
        call writearray_bin3D(cube,imsize,imsize,nchan,filen,'mv')

        return
        end

