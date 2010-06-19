c! Creates the sky with sources.
c! adding noise seperately outside tho it will waste time, for flexibility.
c! Break up source counts into 5 bins, fit power law to each, invert, calc analytically fn of rand.

        subroutine cr8sky(freq,bw,synbeam,cdelt1,imsize,bmsampl,xsize,
     /    nchan,sens,cn1,sarr,gam1,nbin,seed,fn,std,srldir,
     /    beamvary,bmvaryf,bmvaryn)
        implicit none
        include "constants.inc"
        integer xsize
        integer nbin,i,j,imsize,seed,simsize,xc,yc
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
        real*8 bmvaryf(bmvaryn)

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
          call getrandposn(imsize,imsize,xposn,yposn,seed)   ! get random centre
          call getrandsize(flux,freq,sizeas,seed)   ! size in asec from empirical reln.
          sizepix=sizeas/cdelt1

c        do 100 i=5,imsize,40
c         do 110 j=5,imsize,40
c         flux=0.2d0
c        xposn=i*1.d0
c        yposn=j*1.d0
c        sizepix=3.d0
c        sizeas=sizepix*cdelt1

          bmin=bmsampl*cdelt1
          bmaj=sqrt(bmsampl*cdelt1*bmsampl*cdelt1+sizeas*sizeas)  ! hack
          bmajpix=bmaj/cdelt1
          if (sizepix.gt.0.2*bmsampl.and.flux.ge.3.d0*sens) then
           call ran1(seed,rand)
           bpa=rand*180.d0   ! 0-180 deg
          else
           bpa=0.d0
          end if

c! get convolved subimage of source to add to image
c! simsize is int(size convl subim)
          call get_subim_imsize(max(bmaj/cdelt1,bmin/cdelt1),flux,   ! bmaj/cdelt is in pix
     /         sens,simsize,bmvaryn,bmvaryf,beamvary,imsize) 
          if (simsize.gt.xsize) 
     /              write (*,*) '  ### ERROR : Image too big'

c! get the image, of pt src of flux, convled with fw=bmsampl gauss at x,y of size simsize
c! round(xposn,yposn) == (xc,yc)
c@@@@@         if (flux.le.2.d0*sens.or.bmaj/bmin.le.1.1d0) then
          call get_subim_ptsrc(fluxim,xsize,simsize,bmsampl,
     /         flux,xposn,yposn,xc,yc,bmvaryn,bmvaryf,beamvary,imsize)

c####        else
c! Do like above for point source, but this time for proper extended source distribution
c          call get_subim_xsrc(fluxim,xsize,simsize,bmajpix,
c     /                       bmsampl,flux,bpa,xposn,yposn,xc,yc)

c! Or assume is a double source with seperation=size and each component as pt src.
c####          call get_subim_dubsrc(fluxim,xsize,simsize,sizepix,bmsampl,
c####     /         flux,bpa,xposn,yposn,xc,yc,seed,xc1,yc1,xc2,yc2,rand)

c####         end if

c! now add the subimage to the full image
          xca=round(xposn)-xc
          yca=round(yposn)-yc
          do i1=1,simsize
           do j1=1,simsize
            if (xca+i1.ge.1.and.xca+i1.le.imsize.and.
     /          yca+j1.ge.1.and.yca+j1.le.imsize)
     /       image(xca+i1,yca+j1)=image(xca+i1,yca+j1)+fluxim(i1,j1)
           end do
          end do
          
c! write source list
          if (flux.ge.5.d0*sens) then
           if (flux.le.2.d0*sens.or.bmaj/bmin.le.1.1d0) then
            call writesrclist(21,xposn,yposn,flux,bmin,bmin,bpa)
           else
            call writesrclist(21,xca+xc1,yca+yc1,rand*flux,bmin,
     /           bmin,bpa)
            call writesrclist(21,xca+xc2,yca+yc2,(1.d0-rand)*flux,
     /           bmin,bmin,bpa)
           end if
          end if
 
c          if (nsrc(i).gt.5.d5.and.mod(j,1000000).eq.0) 
c     /       write (*,'(a1,$)') '.'

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

