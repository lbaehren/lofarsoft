c! This package simulates the LOFAR (and other) skies. 
c! Uses subroutines of the package noise, is graphics free. Needs g77 and cfitsio.
c! (Put GNU license here)
c!                                      Niruj Mohan Ramanujam
c!                                      Sterrewacht.
c!                                      mohan@strw.leidenuniv.nl

c! Aug 2006 -   Morphology : double sources are two pt srcs.
c!              Writes out FITS file. 


        implicit none
        include "includes/constants.inc"
        integer seed,imsize,nchan,simsize,nchar
        real*8 area,time,bw,freq,bmsampl,dia
        real*8 sens,bl,synbeam,pribeam
        real*8 sarr(500),cdfarr(500),cn1(500),gam1(500)
        real*8 cdelt1,cdelt2,std
        real*8 crval1,crval2,crpix1,crpix2
        character fn*500,f3*500,fitsdir*500,scratch*500,runcode*2
        character beamvary
        integer bmvaryn
        parameter (bmvaryn=10)
        real*8 bmvaryf(bmvaryn)

        data bmsampl/3.d0/   ! 3 pixels in beam

        call getseed(seed)

        call read_simparadefine(fitsdir,scratch,runcode)

c! survey para is sky area, int time, bw. calc above which flux to store srclist
        call getsurveypara(area,time,bw,nchan,freq,bl,fn,dia,beamvary)  ! str, secs, Hz, #, Hz, m
        call getsens(freq,sens)
        sens=sens/sqrt(bw/nchan*1.d-6*time/3600.d0)  ! in Jy, store from here.

c! calc other para
        synbeam=c/freq/bl    ! in rad
        cdelt1=synbeam*rad*3600.d0/bmsampl ! in asec, pix per fwhm
        cdelt2=cdelt1
        pribeam=c/freq/dia      ! in rad
        imsize=int(sqrt(area)*rad*3600.d0/cdelt1)

        write (*,*) 
        write (*,'(a14,f6.2,a9)') '   Cellsize = ',cdelt1,' asec/pix'
        write (*,'(a14,i7)') '   Imsize   = ',imsize
        write (*,*) 
        
c! get (S,N) array for the survey
        call getcdf(area,freq,sarr,cdfarr) ! use -0.8, get (Jy, N(>S))
        if (sens.le.sarr(1).or.sens.gt.sarr(500)/3.d0) then
         write (*,*) '  ### Need to extrapolate number counts'
         write (*,*) '  ###           Exiting '
         write (*,*) sarr(1),sens,sarr(500)
         goto 334
        end if

c! fit n power laws to numb cts
        call fitinparts(sarr,cdfarr,100,cn1,gam1) ! break into 500/nbin bits, fit each

c! spatial variation of beam. 
c! First simple model is sinusoidal+plane variation in sigma (above sigma_0) with cons DF
c! f's are f1,f2,delta_sig/si_o, n_x, n_y (look at notes)
        call set_varypsf(fn,beamvary,bmvaryf,bmvaryn,seed,imsize,
     /       imsize,bmsampl,cdelt1,cdelt2)

c! now obtain flux, x, y, populate into image, prepare source list 
        call cr8sky(freq,bw,synbeam,cdelt1,imsize,bmsampl,
     /       min(50*int(bmsampl),imsize),nchan,sens,cn1,sarr,
     /       gam1,100,seed,fn,std,scratch,beamvary,bmvaryf,bmvaryn)

c! create noise map seperately, convolve with gaussian. add src+noi later.
        call sim_cr8noise(fn,sens,nchan,imsize,bmsampl,seed)

c! fix coordinate system and other keywords
        call get_fitskeywords(imsize,crval1,crval2,crpix1,crpix2)

c! fix pointing centres
        call get_mosaicpara(fn,imsize,scratch)

c! now de-mosaic the image and add src and noise to create subcubes, write fits directly.
c! also do total image as .img.
        call sim_addnoisrc(fn,imsize,nchan,freq,synbeam,crval1,crval2,
     /               crpix1,crpix2,cdelt1,cdelt2,bw,scratch,fitsdir)

334     continue
        write (*,777) '  Thermal noise   = ',sens*1.d3,' mJy'
        write (*,777) '  Confusion noise = ',std*1.d3,' mJy'
        write (*,*)
777     format(a21,f7.3,a4)

c        call system('./noise')
        
        end

