c! read external catalogue and extract flux at that posn from an image.
c! catalogue format is : hh mm ss sdd ma sa sizedeg beamdeg fluxmJy freqMHz
c! assume bdsm has been run.

        subroutine externalcat(catfile,fitsfile,fitsdir,scratch,srldir,
                   runcode)
        implicit none
        include "wcs_bdsm.inc"
        character catfile*500,fitsfile*500,fitsdir*500,scratch*500
        character srldir*500,filename*500,extn*20,solnname*500
        character fn*500,chr1*1,ctype(3)*8,rmsd*500,bdsmsolnname*500
        character fullname*500,gausshap*500,dumc*500
        integer nsrc,n,m,l,boxsize,stepsize,nchar,blankn,i,j,outsideuniv
        logical exists,isgoodpix
        real*8 crpix(3),cdelt(3),crval(3),crota(3),bm_pix(3),blankv,dumr
        integer wcslen,radius,ngau
        parameter (wcslen=450)
        integer wcs(wcslen)

        filename=''
        if (fitsname(nchar(fitsname)-4:nchar(fitsname)).eq.'.FITS'.or.
     /     fitsname(nchar(fitsname)-4:nchar(fitsname)).eq.'.fits') then
         filename=fitsname(1:nchar(fitsname)-5)
        else
         filename=fitsname
        end if
        call callreadfits(fitsname,filename,filename,fitsdir,
     /       scratch,runcode)
        extn='.img'
        call readarraysize(filename,extn,n,m,l)
        call getline(catfile,srldir,nsrc)

        write (*,'(a,$)') 
     /     '  How many effective beam radii to search in (e.g. 1.5) : '
        read (*,*) radius
333     write (*,'(a,$)') 
     /    '   Enter solnname for rmsd file (none=compute) : '
        read (*,*) solnname
        if (solnname(1:nchar(solnname)).eq.'none') then
444      write (*,'(a,$)') '   (c)onst or c(a)lculate : '
         read (*,*) chr1
         if (chr1.ne.'c'.and.chr1.ne.'a') goto 444
         if (chr1.eq.'a') then                      ! calculate rmsd image
          write (*,*) '  boxsize, stepsize : '
          read (*,*) boxsize,stepsize
          extn='.img'
          blankv=-999.d0                         
          call get_numpix_val(filename,extn,n,m,blankv,'eq',blankn)
          fn=filename(1:nchar(filename))//'.header'
          call read_head_coord(fn(1:nchar(fn)),3,
     /         ctype,crpix,cdelt,crval,crota,bm_pix,scratch)
          call wcs_struct(3,ctype,crpix,cdelt,crval,crota,wcs,wcslen)
          outsideuniv=0
          do j=1,m
           do i=1,n
            call wcs_isgoodpix(i*1.d0,j*1.d0,wcs,wcslen,isgoodpix)
            if (.not.isgoodpix) outsideuniv=outsideuniv+1
           end do
          end do
          imagename=filename(1:nchar(filename))//'.calc'
          if (blankn.eq.0) then
           call bdsm_boxnoise(filename,imagename,boxsize,stepsize,
     /          n,m,runcode,outsideuniv_num,wcslen,wcs)
          else
           call bdsm_boxnoisemask(filename,imagename,boxsize,stepsize,
     /          n,m,runcode,blankv,outsideuniv_num,wcslen,wcs)
          end if
          call freewcs(wcs,wcslen)
          rmsd='calc'
         else                                      ! take constant rms
          rmsd='none'
         end if
        else                                       ! take pre-existing file
         fn=scratch(1:nchar(scratch))//filename(1:nchar(filename))//
     /      '.'//solnname(1:nchar(solnname))//'.rmsd.img'
         inquire(file=fn,exist=exists)
         if (.not.exists) then
          write (*,*) '  No rmsd file with that solnname'
          goto 333
         end if
         rmsd=solnname
        end if
        rmsd=''  filename for rmsd; none=const

        write (*,'(a,$)') 
     /    '   Enter solnname of the BDSM run : '
        read (*,*) bdsmsolnname
        fullname=filename(1:nchar(filename))//'.'//
                 bdsmsolnname(1:nchar(bdsmsolnname))
        gausshap='10'
        call ranbdsm1(scratch,srldir,fitsdir,fullname,gausshap)
        if (.not.ranbdsm1) then
         write (*,*) ' BDSM not run properly with that solnname'
        else
         fn=filename(1:nchar(filename))//'.'//bdsmsolnname(1:
     /      nchar(bdsmsolnname))//'.gaul'
         open(unit=21,file=fn(1:nchar(fn)),form='formatted')
         call readhead_srclist(21,19,'Number_of_gaussians',dumc,ngau,
     /        dumr,'i')
         close(21)
         call sub_ec(catfile,srldir,scratch,filename,nsrc,n,m,rmsd,
     /        blankn,radius,bdsmsolnname,ngau)
        end if

        return
        end
c!
c!
c!
        subroutine sub_ec(catfile,srldir,scratch,filename,nsrc,n,m,
     /             rmsd,blankn,radius,bdsmsolnname,ngau)
        implicit none
        include "wcs_bdsm.inc"
        character catfile*500,srldir*500,scratch*500,filename*500
        character f1*500,ctype(3)*8,rmsd*500,bdsmsolnname*500,fn*500
        character dumc*500
        integer nsrc,i,n,m,err,blankn,ngau
        real*8 hh,mm,ss,dd,ma,sa,cbeam(nsrc),image(n,m),radius
        real*8 cra(nsrc),cdec(nsrc),cflux(nsrc),cfreq(nsrc),csize(nsrc)
        real*8 crpix(3),cdelt(3),crval(3),crota(3),bm_pix(3),crms(nsrc)
        real*8 x,y,pk1,tot1,kappa,std,av,stdclip,avclip,dumr
        real*8 mx,mn,mxv,mnv,rms(n,m),gaus(6,10),egaus(6,10)
        real*8 gaus_o(3),gaus_r(3),gaus_d(3),gaus_c(3),kappa
        integer wcslen
        parameter (wcslen=450)
        integer wcs(wcslen)
        logical isgoodpix,blanky
        integer flag(ngau),srcid(ngau),trc1(ngau),trc2(ngau)
        integer gaulid(ngau),islid(ngau),blc1(ngau),blc2(ngau)
        real*8 ra(ngau),dec(ngau),rstd(ngau),rav(ngau),sstd(ngau)
        real*8 sav(ngau),tot(ngau),dtot(ngau)
        real*8 bmaj(ngau),bmin(ngau),bpa(ngau),xpix(ngau)
        real*8 expix(ngau),ypix(ngau),eypix(ngau)
        real*8 era(ngau),edec(ngau),peak(ngau)
        real*8 epeak(ngau),ebmaj(ngau),ebmin(ngau),ebpa(ngau)
        real*8 dbmaj(ngau),edbmaj(ngau),dbmin(ngau),edbmin(ngau)
        real*8 dbpa(ngau),edbpa(ngau),chisq(ngau),q(ngau)
        character code(ngau)*4,code4(ngau)*4,names(ngau)*30
        real*8 im_rms(ngau),dumr2(ngau),dumr3(ngau),dumr4(ngau)
        real*8 dumr5(ngau),dumr6(ngau)
        real*8 xim(ngau),yim(ngau)

        kappa=2.d0

c! read external catalogue file
        open(unit=21,file=srldir(1:nchar(srldir))//
     /                    catfile(1:nchar(catfile)))
        do i=1,nsrc
         read (21,*) hh,mm,ss,dd,ma,sa,csize(i),cbeam(i),
     /               cflux(i),crms(i),cfreq(i)
         call convertdecinv(dd,ma,sa,cdec(i))
         cra(i)=(hh+mm/60.d0+ss/3600.d0)*15.d0
        end do
        close(21)

c! prepare wcs
        f1=filename(1:nchar(filename))//'.header'
        call read_head_coord(f1(1:nchar(f1)),3,
     /     ctype,crpix,cdelt,crval,crota,bm_pix,scratch)
        call wcs_struct(3,ctype,crpix,cdelt,crval,crota,wcs,wcslen)
        gaus_c(1)=abs(cdelt(1))*bm_pix(1)
        gaus_c(2)=abs(cdelt(2))*bm_pix(2)
        gaus_c(3)=bm_pix(3)

c! read in image and prepare rmsd image 
        extn='.img'
        call readarray_bin(n,m,image,n,m,filename,extn)
        if (rmsd(1:nchar(rmsd)).eq.'none') then
         blanky=.false.
         if (blankn.gt.0) blanky=.true.
         call get_imagestats2D(filename,4.d0,n,m,std,av,stdclip,avclip,
     /        mx,mn,mxv,mnv,blanky,blankv)
         call initialiseimage(rms,n,m,n,m,stdclip)
        else
         extn='.img'
         f1=filename(1:nchar(filename))//'.'//rmsd(1:nchar(rmsd))
         call readarray_bin(n,m,rms,n,m,f1,extn)
        end if

c! read gaul
        dumc=filename(1:nchar(filename))//'.'//
                 bdsmsolnname(1:nchar(bdsmsolnname))
        call readgaul(dumc,ngau,ngau,srldir,gaulid,islid,flag,tot,
     /    dtot,peak,
     /    epeak,ra,era,dec,edec,xpix,expix,ypix,eypix,bmaj,ebmaj,
     /    bmin,ebmin,bpa,ebpa,dbmaj,edbmaj,dbmin,edbmin,dbpa,edbpa,
     /    sstd,sav,rstd,rav,chisq,q,code,names,blc1,blc2,
     /    trc1,trc2,im_rms,dumr2,dumr3,dumr4,dumr5,dumr6)
        do igau=1,ngau
         call wcs_radec2xy(ra(igau),dec(igau),xim(igau),yim(igau),err,
     /        wcs,wcslen) 
        end do

c! analyse for each source in catalogue
        do i=1,nsrc
         ra=cra(i)
         dec=cdec(i)
c! get reconvolved size of source (after deconvolving cat beam)
         e_maj=cbeam(i)/(2.d0*cflux(i)/crms(i))
         e_min=cbeam(i)/(2.d0*cflux(i)/crms(i))
         e_pa=0.d0
         gaus_o(1)=csize(i)
         gaus_o(2)=csize(i)
         gaus_o(3)=0.d0
         gaus_r(1)=cbeam(i)
         gaus_r(2)=cbeam(i)
         gaus_r(3)=0.d0
         call deconv(gaus_o,gaus_r,gaus_d,e_maj,e_min,
     /        e_pa,e_d1,e_d2,e_d3,error)   ! size deconvolved with cat beam
         call reconv(gaus_d,gaus_c,gaus_o,e_d1,e_d2,
     /        e_d3,e_maj,e_min,e_pa,error)   ! size convolved with image beam
         size_pix(1)=gaus_o(1)/abs(cdelt(1))
         size_pix(2)=gaus_o(2)/abs(cdelt(2))
         size_pix(3)=gaus_o(3)
         call wcs_radec2xy(ra,dec,x,y,err,wcs,wcslen) 
         call wcs_isgoodpix(x,y,wcs,wcslen,isgoodpix)
         if (isgoodpix) then
          call extract_srcprop(image,n,m,size_pix,bm_pix,x,y,pk1,tot1)
          call extract_aroundsrc(image,n,m,bm_pix,cbeam(i)/abs(
     /      cdelt(1)*cdelt(2)),x,y,gaus,egaus,radius,kappa,rms,blankv,
     /      filename,solnname,ngau,flag,tot,dtot,peak,epeak,ra,era,dec,
     /      edec,bmaj,ebmaj,bmin,ebmin,bpa,ebpa,dbmaj,edbmaj,dbmin,
     /      edbmin,dbpa,edbpa,code,im_rms,names,wcs,wcslen,xim,yim)
         end if
        end do

        return
        end
c!
c!
c! Just put a beam on the (x,y) and get peak and total
        subroutine extract_srcprop(image,n,m,size_pix,bm_pix,x,y,
     /             pk1,tot1)
        implicit none
        includes "constants.inc"
        integer n,m,ix,iy,i,j,num
        real*8 image(n,m),bm_pix,x,y,pk1,tot1,xpix(2),ypix(2)
        real*8 bmaj(2),bmin(2),bpa(2),fwhm,tot
        real*8 e_maj,e_min,e_pa,size_pix(3)
        
        do i=1,2
         bmaj(i)=size_pix(1)
         bmin(i)=size_pix(2)
         bpa(i)=size_pix(3)
        end do
        xpix(1)=x
        ypix(1)=y
        num=0
        tot=0.d0
        size1=round(max(size_pix(1),size_pix(2))*1.5d0/2.d0)    ! take this as outer limit
        do i=round(x)-size1,round(x)+size1
         do j=round(y)-size1,round(y)+size1
          dist=sqrt((x-i)*(x-i)+(y-j)*(y-j))
          xpix(2)=i*1.d0
          ypix(2)=j*1.d0
          call gdist_pa(1,2,2,xpix,ypix,bmaj,bmin,bpa,fwhm)   ! dist to pix along PA of size
          if (dist+0.5d0*sqrt(2.d0).le.fwhm/2.d0) then
           tot=tot+image(i,j)
           num=num+1 
          end if
         end do
        end do

        bmpix=bm_pix(1)*bm_pix(2)*2.d0*pi/fwsig/fwsig   ! number of pixels in 1 beam 
        tot1=tot*num/bmpix

        return
        end
c!
c!
c!  peaks within radius*sqrt(bm1^2+bm2^2)
        subroutine extract_aroundsrc(image,n,m,bm_pix,catbm,x,y,gaus,
     /      egaus,radius,kappa,rmsim,blankv,filename,solnname,
     /      ngau,flag,tot,dtot,peak,epeak,ra,era,dec,edec,bmaj,ebmaj,
     /      bmin,ebmin,bpa,ebpa,dbmaj,edbmaj,dbmin,edbmin,dbpa,edbpa,
     /      code,im_rms,names,wcs,wcslen,xim,yim)
        implicit none
        integer n,m,dumi,maxline,ngau,siz,wcslen,wcs(wcslen)
        real*8 gaus(6,10),egaus(6,10),bm_pix(3),image(n,m),dumr
        real*8 kappa,catbm,rmsim(n,m),rms,blankv
        character filename*500,solnname*500
        integer flag(ngau)
        real*8 ra(ngau),dec(ngau),tot(ngau),dtot(ngau)
        real*8 bmaj(ngau),bmin(ngau),bpa(ngau),xim(ngau),yim(ngau)
        real*8 era(ngau),edec(ngau),peak(ngau)
        real*8 epeak(ngau),ebmaj(ngau),ebmin(ngau),ebpa(ngau)
        real*8 dbmaj(ngau),edbmaj(ngau),dbmin(ngau),edbmin(ngau)
        real*8 dbpa(ngau),edbpa(ngau),im_rms(ngau)
        character code(ngau)*4,names(ngau)*30

c! radius to search in; X2 just in case
        dumr=max(bm_pix(1)*bm_pix(2))
        siz=int(sqrt(dumr*dumr+catbm*catbm)*radius*0.5d0)*2

c! get average rms
        rms=0.d0
        dumi=0
        do i=round(x)-siz,round(x)+siz
         do j=round(y)-siz,round(y)+siz
          if (image(i,j).ne.blankv) then
           dumi=dumi+1
           rms=rms+rmsim(i,j)
          end if
         end do
         rms=rms/dumi

c! easiest is to assume bdsm has been run on the image already.
c! just give them all the gaussians inside some area around posn.
        numgaus=0
        do i=round(x)-siz/2,round(x)+siz/2
         do j=round(y)-siz/2,round(y)+siz/2
          if (image(i,j).ne.blankv) then
           do igau=1,ngau
            dist=sqrt((xp(1)-xp(2))*(xp(1)-xp(2))+
     /                (yp(1)-yp(2))*(yp(1)-yp(2)))
            if (dist.le.siz/2.d0)
             numgaus=numgaus+1
             ngau(numgaus)=igau
           end if
          end if
         end do
        end do

        return
        end
c!
c!
c!





