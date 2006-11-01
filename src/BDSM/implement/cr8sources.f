c! this creates sources, convolves with different psfs and puts noise, and makes the
c! source list. To test BDSM with, exactly. copy most stuff from simulation.f

        subroutine cr8sources(filename,seed,scratch)
        implicit none
        include "constants.inc"
        character filename*500,scratch*500
        real*8 fwhm,beampersrc,snrmin,snrmax,bmareamin,bmareamax
        real*8 dumr1,dumr2,ratmin,ratmax,dumr
        integer n,m,seed,maxsize,round

        write (*,*) '  This will create an image and a sourcelist.'
        write (*,*) 
        call sub_cr8sources_gethead(n,m,fwhm,beampersrc,snrmin,
     /             snrmax,bmareamin,bmareamax,ratmin,ratmax)

        dumr=fwhm*sqrt(ratmax*bmareamax)
        maxsize=2*round(dumr/fwsig*sqrt(2.d0*dlog(snrmax/0.3d0)))+1  ! down to 0.3 sigma
        call sub_cr8sources(filename,n,m,fwhm,beampersrc,snrmin,
     /    snrmax,bmareamin,bmareamax,ratmin,ratmax,seed,maxsize,scratch)
        
        return
        end

c!      --------------------------------   SUBROUTINES   -------------------------------

        subroutine sub_cr8sources(filename,n,m,fwhm,beampersrc,snrmin,
     /     snrmax,bmareamin,bmareamax,ratmin,ratmax,seed,msize,scratch)
        implicit none
        include "constants.inc"
        integer n,m,nsrc,round,seed,msize,xc,yc,xca,yca
        integer nchar,isrc,i,j,dumi,wid,flag(1)
        real*8 fwhm,beampersrc,snrmin,snrmax,bmareamin,bmareamax
        real*8 image(n,m),bmarea,ratmin,ratmax,subim(msize,msize)
        real*8 rand,area,ratio,a(6),cnoise(n,m),fac,s1,bm_pix(3)
        character filename*500,fn*500,prog*20,ffmt*500,scratch*500
        real*8 crpix(3),cdelt(3),crval(3),crota(3)
        real*8 noise(n,m),sig_c(3),std,av,deconv_s(3,1)
        character ctype(3)*8,bcon*10,bcode*4,rmsmap*500,keyword*500
        character extn*10,comment*500,flag_s(1)
        real*8 e_amp(1),e_x0(1),e_y0(1),e_maj(1),e_min(1)
        real*8 e_pa(1),e_tot(1)

c! first calculate parameters for the image
        bmarea=2.d0*pi/fwsig/fwsig*fwhm*fwhm  ! in pix^2
        nsrc=round(n*m/(beampersrc*bmarea))
        if (nsrc.lt.1) nsrc=1
        write (*,*) '  Creating ',nsrc,' sources'

        extn='.bstat'
        keyword='rms_map'
        rmsmap='const'
        comment=" 'Use constant rms value or rms map'"
        call put_keyword(filename,extn,keyword,rmsmap,0.d0,
     /       comment,'S',scratch)
        keyword='thresh_pix'
        comment=" 'Value in sigma for hard threshold'"
        call put_keyword(filename,extn,keyword,keyword,4.d0,
     /       comment,'R',scratch)

c! initialise image, open sourcelist file (.ini.gaul == real srl), fix headers
        call initialiseimage(image,n,m,n,m,0.d0)
        fn=scratch(1:nchar(scratch))//filename(1:nchar(filename))//
     /     '.ini.gaul'
        open(unit=23,file=fn(1:nchar(fn)),status='unknown')
        open(unit=27,file=fn(1:nchar(fn)),form='unformatted') 
        prog='cr8sources.f'
        call sub_sourcemeasure_writehead(23,filename,filename,n,m,nsrc,
     /       1.d0,snrmin,prog)
        call read_dum_head(3,ctype,crpix,cdelt,crval,crota,bm_pix)
        crpix(1)=int(n/2)
        crpix(2)=int(m/2)
        call writefitshead(filename,3,ctype,crpix,cdelt,crval,crota
     /       ,bm_pix,scratch)

c! for each source ... 
        dumi=0  ! hack for put_srclist
        do 100 isrc=1,nsrc

c! get random source parameters 
         call sub_cr8sources_rand_a(seed,a,snrmin,snrmax,n,m,bmareamin,
     /        bmareamax,ratmin,ratmax,bmarea)

c! get the subimage of delta fn convolved with elliptical gaussian
         call get_subim_xsrc(subim,msize,msize,a(4)*fwsig,a(5)*fwsig,
     /       a(1),a(6),a(2),a(3),xc,yc)
        
c! add subimage to image (copied from cr8sky.f)
         xca=round(a(2))-xc
         yca=round(a(3))-yc
         do i=1,msize
          do j=1,msize
           if (xca+i.ge.1.and.xca+i.le.n.and.
     /         yca+j.ge.1.and.yca+j.le.m)
     /      image(xca+i,yca+j)=image(xca+i,yca+j)+subim(i,j)
          end do
         end do

c! write source list
         if (a(6).le.90.d0) then 
          a(6)=a(6)+90.d0
         else
          if (a(6).gt.90.d0.and.a(6).le.180.d0) then 
           a(6)=a(6)-90.d0
          end if
         end if
         deconv_s(1,1)=0.d0
         deconv_s(2,1)=0.d0
         deconv_s(3,1)=0.d0
         flag(1)=0
         flag_s(1)=' '
         e_amp(1)=0.d0
         e_x0(1)=0.d0
         e_y0(1)=0.d0
         e_maj(1)=0.d0
         e_min(1)=0.d0
         e_pa(1)=0.d0
         e_tot(1)=0.d0
         call putin_srclist(23,27,isrc,dumi,a,1,6,1,0.00,0.d0,0.d0,0.d0,
     /     0.d0,0.d0,0,0,ffmt,filename,ctype,crpix,cdelt,crval,crota,
     /     deconv_s,0,flag,flag_s,e_amp,e_x0,e_y0,e_maj,e_min,e_pa,
     /     e_tot,scratch,bm_pix)

100     continue  !  for each source
        close(23)
        close(27)
        call sub_sourcemeasure_writeheadmore(nsrc,1,fn,ffmt,scratch)

c! create noise map
        sig_c(1)=fwhm/fwsig
        sig_c(2)=fwhm/fwsig
        sig_c(3)=0.d0
        call conv2_get_wid_fac(sig_c,n,m,s1,fac,wid)
        call xcr8noisemap(n,m,n,m,1.d0/fac,noise,seed)
        bcon='periodic'
        bcode='bare'                       ! total is preserved
        call conv2(noise,n,m,n,m,sig_c,cnoise,bcon,bcode,fac,s1)
        fn=filename(1:nchar(filename))//'.noisemap'
        call writearray_bin(cnoise,n,m,n,m,fn,'mv')

c! add noise map to source map and write out
        do i=1,n
         do j=1,m
          image(i,j)=image(i,j)+cnoise(i,j)
         end do
        end do
        call writearray_bin(image,n,m,n,m,filename,'mv')
        write (*,*) '  Writing sourcelist ',filename(1:nchar(filename))
     /              //'.ini.gaul'

        return
        end
c!
c!      ==========
c!
        subroutine sub_cr8sources_gethead(n,m,fwhm,beampersrc,snrmin,
     /             snrmax,bmareamin,bmareamax,ratmin,ratmax)
        implicit none
        real*8 fwhm,beampersrc,snrmin,snrmax,bmareamin,bmareamax
        real*8 dumr1,dumr2,ratmin,ratmax
        integer n,m

111     write (*,'(a,$)') '   Size of image (n X m) : '
        read (*,*) n,m
        if (n.le.10.or.m.le.10) goto 111
222     write (*,'(a,$)') '   Nominal beam fwhm (pixels) : '
        read (*,*) fwhm
        if (fwhm.gt.(max(n,m)/2.d0).or.fwhm.le.1.d0) goto 222
        write (*,'(a,$)') '   Number of nominal beams per source : '
        read (*,*) beampersrc
        write (*,'(a,$)') '   Range of SNR of sources : '
        read (*,*) dumr1,dumr2
        snrmin=min(dumr1,dumr2)
        snrmax=max(dumr1,dumr2)
        write (*,'(a,$)') '   Range of beam area (factor on nominal) : '
        read (*,*) dumr1,dumr2
        bmareamin=min(dumr1,dumr2)
        bmareamax=max(dumr1,dumr2)
        write (*,'(a,$)') '   Range of major/minor axis ratio : '
        read (*,*) dumr1,dumr2
        ratmin=min(dumr1,dumr2)
        ratmax=max(dumr1,dumr2)
        
        return
        end
c!
c!      ----------------
c!
        subroutine sub_cr8sources_rand_a(seed,a,snrmin,snrmax,n,m,
     /        bmareamin,bmareamax,ratmin,ratmax,bmarea)
        implicit none
        include "constants.inc"
        integer n,m,seed
        real*8 fwhm,beampersrc,snrmin,snrmax,bmareamin,bmareamax
        real*8 a(6),rand,ratio,area,ratmin,ratmax,bmarea

        call ran1(seed,rand)
        a(1)=rand*(snrmax-snrmin)+snrmin      ! flux in units of snr
        call ran1(seed,rand)
        a(2)=rand*(n-1)+1
        call ran1(seed,rand)
        a(3)=rand*(m-1)+1
        call ran1(seed,rand)
        area=(rand*(bmareamax-bmareamin)+bmareamin)*bmarea
        call ran1(seed,rand)
        ratio=rand*(ratmax-ratmin)+ratmin      ! bmaj/bmin
        a(4)=sqrt(area*0.5d0/pi*ratio)
        a(5)=sqrt(area*0.5d0/pi/ratio)  ! sig(bmaj,bmin) in pixels
        call ran1(seed,rand)
        a(6)=rand*180.d0                       ! bpa in deg
        
        return
        end
c!
