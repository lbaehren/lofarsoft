c! this is to do basic quality checks, and set up input parameters for 
c! an automated running of bdsm.
c! num of bm/src is approx n*m/(num pix > 5 sigma)/(1-alpha), alpha is diff src ct, +ve

        subroutine preprocess(f1,fname,imagename,runcode,scratch,
     /             n,m,l0,l)
        implicit none
        integer nchar,n,m,error,l,l0,dumi
        real*8 crpix(3),cdelt(3),crval(3),crota(3),bm_pix(3),keyvalue
        character f1*500,fn*500,ctype(3)*8,extn*20,ch1*1,scratch*500
        character runcode*2,f2*500,keystrng*500,fg*500,dir*500,fname*500
        real*8 bmpersrc_th,boxsize,stepsize,fitfreely
        real*8 minpix_isl,maxsize_beam,snrclip,dumr
        character comment*500,keyword*500,imagename*500
        character gausshap*500,iniguess*500,flagsmallsrc*500
        logical exists
        
        if (runcode(2:2).eq.'q') write (*,*) '  Preprocessing image'

        extn='.header'
        fn=scratch(1:nchar(scratch))//fname(1:nchar(fname))//
     /     extn(1:nchar(extn))
        inquire(file=fn,exist=exists)
        call read_dum_head(3,ctype,crpix,cdelt,crval,crota,bm_pix)
        if (exists) then 
         f2=fname(1:nchar(fname))//extn(1:nchar(extn))
         call read_head_coord(f2(1:nchar(f2)),3,
     /        ctype,crpix,cdelt,crval,crota,bm_pix,scratch)
        else
         call writefitshead(fname,3,crpix,ctype(1),ctype(2),ctype(3),
     /       cdelt,crval,crota,bm_pix,scratch)
        end if
        if (l.gt.1) write (*,*) '  Using 2d array for 3d image !!!'

c! get other keys from paradefine
        fg="paradefine"
        extn=""
        dir="./"
        keyword="bmpersrc_th"
        call get_keyword(fg,extn,keyword,keystrng,bmpersrc_th,
     /    comment,"r",dir,error)

        keyword="boxsize_th"
        call get_keyword(fg,extn,keyword,keystrng,boxsize,
     /    comment,"r",dir,error)
        if (boxsize.lt.2.d0) boxsize=0.d0
        if (boxsize.gt.max(n,m)*1.d0) boxsize=max(n,m)*1.d0

        keyword="stepsize_th"
        call get_keyword(fg,extn,keyword,keystrng,stepsize,
     /    comment,"r",dir,error)
        if (stepsize.lt.1.d0) stepsize=0.d0
        if (stepsize.gt.min(n,m)*1.d0) stepsize=0.d0

        keyword="minpix_isl"
        call get_keyword(fg,extn,keyword,keystrng,minpix_isl,
     /    comment,"r",dir,error)
        
        keyword="maxsize_beam"
        call get_keyword(fg,extn,keyword,keystrng,maxsize_beam,
     /    comment,"r",dir,error)

        keyword="fitfreely"
        call get_keyword(fg,extn,keyword,keystrng,fitfreely,
     /    comment,"r",dir,error)
        if (fitfreely.ne.0) fitfreely=1.d0

        keyword="iniguess"
        call get_keyword(fg,extn,keyword,iniguess,keyvalue,
     /    comment,"s",dir,error)
        if (iniguess(1:nchar(iniguess)).ne.'all') iniguess='default'

        keyword="flagsmallsrc"
        call get_keyword(fg,extn,keyword,flagsmallsrc,keyvalue,
     /    comment,"s",dir,error)
        if (flagsmallsrc(1:nchar(flagsmallsrc)).ne.'true'.and.
     /      flagsmallsrc(1:nchar(flagsmallsrc)).ne.'false') 
     /      flagsmallsrc='true'

        extn=''
        keyword="gausshap"
        call get_keyword(fg,extn,keyword,gausshap,dumr,
     /    comment,"s",dir,error)

        call pp_basicstats(f1,fname,n,m,scratch,imagename,snrclip,  ! writes in 'f1.bstat'
     /       ctype,crpix,cdelt,crval,crota)
        
        call pp_bmpersrc(f1,fname,n,m,scratch,bmpersrc_th,imagename,
     /       snrclip,gausshap)    ! approx formula in notebook
        call pp_imrms_para(f1,fname,n,m,scratch,int(boxsize),
     /       int(stepsize))
        call pp_linearcoords(f1,fname,n,m,scratch,imagename,3,ctype,
     /        crpix,cdelt,crval,crota,bm_pix)

        extn='.bparms'
        if (minpix_isl.le.0.d0) minpix_isl=4.d0
        keyword='minpix_isl'
        comment=" 'Minimum number of pixels in an island'"
        call put_keyword(fname,extn,keyword,keyword,minpix_isl,
     /       comment,'R',scratch)
        if (maxsize_beam.le.0.d0) maxsize_beam=10.d0
        keyword='maxsize_beam'
        comment=" 'Maximum size of fitted gaussian in beams'"
        call put_keyword(fname,extn,keyword,keyword,maxsize_beam,
     /       comment,'R',scratch)

        extn='.bparms'
        keyword='fitfreely'
        comment=" ';1 = fit all gaus para.s (default) "//
     /          "0 = fix size=beam'"
        call put_keyword(fname,extn,keyword,keyword,fitfreely,
     /       comment,'R',scratch)
        keyword='iniguess'
        comment=" 'all: take all initial gaussians as guess, "//
     /          "else default'"
        call put_keyword(fname,extn,keyword,iniguess,dumr,
     /       comment,'S',scratch)
        keyword='flagsmallsrc'
        comment=" 'true: flag gaus < 1/2beam, false: keep '"
        call put_keyword(fname,extn,keyword,flagsmallsrc,dumr,
     /       comment,'S',scratch)
        
c! next step
        if (runcode(1:1).eq.'m') then 
         write (*,*) 
         write (*,'(a,$)') '   Continue (noise map : rmsd) (y)/n ? '
         read (*,'(a1)') ch1
         if (ichar(ch1).eq.32) ch1='y'
333      continue
         if (ch1.eq.'y') call callrmsd(f1,runcode,scratch)
        end if
        
        return
        end
c!
c!  ------------------------  SUBROUTINES  -----------------------------------
c!
        subroutine pp_basicstats(f1,fname,n,m,scratch,imagename,
     /             snrclip,ctype,crpix,cdelt,crval,crota)
        implicit none
        include "constants.inc"
        include "wcs_bdsm.inc"
        integer n,m,mx(2),mn(2),nchar,error,blankn,error1
        character f1*500,extn*20,fn*500,keyword*500,fg*500,scratch*500
        character comment*500,code*1,keystrng*500,dir*500,fname*500
        character imagename*500,f2*500,ctype(3)*8
        real*8 std,av,stdclip,avclip,mxv,mnv,kappa,keyvalue
        real*8 blankv,dumr,val,datamax,datamin,snrclip
        logical blanky,isgoodpix
        integer wcslen,xmax,ymax,outsideuniv,i,j
        parameter (wcslen=450)
        integer wcs(wcslen)
        real*8 crpix(3),cdelt(3),crval(3),crota(3)
        
c        keyword='BLANK'                        !!  NO. doesnt mean anything. 
c        call get_keyword(fname,extn,keyword,keystrng,blankv,
c     /       comment,'r',scratch,error)        !!  See eric greisens email.
c        if (error.eq.0) then
c         blanky=.true.

        extn='.img'
        blankv=-999.d0                         ! BDSM value for magic blanking.
        call get_numpix_val(imagename,extn,n,m,blankv,'eq',blankn)
        if (blankn.gt.0) then  
         blanky=.true.
        else
         blanky=.false.
         blankn=0
        end if

        kappa=3.d0
        call get_imagestats2D(imagename,kappa,n,m,std,av,stdclip,avclip,
     /       mx,mn,mxv,mnv,blanky,blankv)

c! create .bparms here as well
        fn=scratch(1:nchar(scratch))//fname(1:nchar(fname))//'.bstat'
        open(unit=21,file=fn,status='unknown')
         f2='image_name = '//imagename(1:nchar(imagename))//
     /                    " 'Name of image'"
         write (21,*) f2(1:nchar(f2))
         write (21,*) 'size_x = ',n," '# pixels on x axis'"
         write (21,*) 'size_y = ',m," '# pixels on y axis'"
         write (21,*) 'mean = ',av," 'Mean over image (BUNIT)'"
         write (21,*) 'rms = ',std," 'RMS over image (BUNIT)'"
         write (21,*) 'kappa_clip = ',kappa," 'Kappa'"
         write (21,*) 'mean_clip = ',avclip,
     /                " 'Mean, kappa-clip (BUNIT)'"
         write (21,*) 'rms_clip = ',stdclip," 'RMS, kappa-clip (BUNIT)'"
         write (21,*) 'max_pixel_x = ',mx(1)," 'x value of max int pix'"
         write (21,*) 'max_pixel_y = ',mx(2)," 'y value of max int pix'"
         write (21,*) 'min_pixel_x = ',mn(1)," 'x value of min int pix'"
         write (21,*) 'min_pixel_y = ',mn(2)," 'y value of min int pix'"
         write (21,*) 'max_value = ',mxv," 'Maximum intensity (BUNIT)'"
         write (21,*) 'min_value = ',mnv," 'Minimum intensity (BUNIT)'"
         write (21,*) 'omega = ',n*m*abs(cdelt(1))*abs(cdelt(2))/
     /          rad/rad," 'Solid angle of image (str)'"
        close(21)
        snrclip=abs(avclip)/stdclip

        fn=scratch(1:nchar(scratch))//fname(1:nchar(fname))//'.bparms'
        open(unit=21,file=fn,status='unknown')
         f2='image_name = '//imagename(1:nchar(imagename))//
     /                    " 'Name of image'"
         write (21,*) f2(1:nchar(f2))
        close(21)

        extn='.bstat'
        keyword='blank_num'
        comment=" 'Number of blanked pixels'"
        dumr=blankn*1.d0
        call put_keyword(fname,extn,keyword,keyword,dumr,
     /      comment,'R',scratch)
        keyword='blank_val'
        comment=" 'Blanking value'"
        call put_keyword(fname,extn,keyword,keyword,blankv,
     /      comment,'R',scratch)

c! now for pixels outside the universe
        call wcs_struct(3,ctype,crpix,cdelt,crval,crota,wcs,wcslen)
        outsideuniv=0
        do j=1,m
         do i=1,n
          call wcs_isgoodpix(i*1.d0,j*1.d0,wcs,wcslen,isgoodpix)
          if (.not.isgoodpix) outsideuniv=outsideuniv+1
         end do
        end do
c        call freewcs(wcs,wcslen)
        extn='.bstat'
        keyword='outsideuniv_num'
        comment=" 'Number of pixels outside universe'"
        dumr=outsideuniv*1.d0
        call put_keyword(fname,extn,keyword,keyword,dumr,
     /      comment,'R',scratch)
        
        return
        end
c!
c!      ----------
c!
        subroutine pp_bmpersrc(f1,fname,n,m,scratch,bmpersrc_th,
     /             imagename,snrclip,gausshap)  ! approx formula in notebook
        implicit none
        integer n,m,numpix,nchar,error
        character f1*500,extn*20,fn*500,keyword*500,fg*500,fname*500
        character comment*500,code*1,keystrng*500,dir*500,scratch*500
        character imagename*500,mean_map*500,gausshap*500,confused*500
        real*8 alpha,rmsclip,bmpersrc_th,keyvalue,blankn
        real*8 blankv,snrclip,dumr
        logical blanky

        if (bmpersrc_th.eq.0.d0) then
         alpha=2.5d0    ! power law of diff src count
         extn='.bstat'
         keyword='rms_clip'
         call get_keyword(fname,extn,keyword,keystrng,rmsclip,
     /        comment,'r',scratch,error)
         keyword='blank_num'
         call get_keyword(fname,extn,keyword,keystrng,blankn,
     /        comment,'r',scratch,error)
 
         extn='.img'
         call get_numpix_val(imagename,extn,n,m,5.d0*rmsclip,'ge',
     /        numpix)
         if (blankn.gt.0.d0) then
          extn='.bstat'
          keyword='blank_val'
          call get_keyword(fname,extn,keyword,keystrng,blankv,
     /         comment,'r',scratch,error)
          if (error.eq.1) write (*,*) 'BLANK error'
          if (blankv.ge.5.d0*rmsclip) numpix=numpix-blankn
         end if
 
         if (numpix.lt.0) write (*,*) ' ERROR ##'
         if (numpix.eq.0) numpix=1
         bmpersrc_th=n*m/((alpha-1.d0)*numpix)
        end if

        extn='.bparms'
        keyword='bmpersrc_th'
        comment=" 'Estimated # beams per source'"
        call put_keyword(fname,extn,keyword,keyword,bmpersrc_th,
     /       comment,'R',scratch)

c! check if av_clip/std_clip is too large or if beam/src is too small, then put av_clip=0
        fg="paradefine"
        extn=""
        dir="./"
        keyword="mean_map"
        call get_keyword(fg,extn,keyword,mean_map,dumr,
     /    comment,"S",dir,error)
        if (mean_map.eq.'default') then
         if (bmpersrc_th.le.25.d0.or.snrclip.ge.0.1d0) then
          confused='true'
         else
          confused='false'
         end if
        else
         confused=' '
        end if

        extn='.bparms'
        dumr=0.d0
        keyword='confused'
        comment=" 'Is the image confused '"
        call put_keyword(fname,extn,keyword,confused,dumr,
     /       comment,'S',scratch)

        keyword='gausshap'
        comment=" "
        call put_keyword(fname,extn,keyword,gausshap,dumr,
     /       comment,'S',scratch)
        
        return
        end
c!
c!      ----------
c!
        subroutine pp_imrms_para(f1,fname,n,m,scratch,boxsize,stepsize)
        implicit none
        include "constants.inc"
        integer n,m,nchar,boxsize,stepsize,conv_filt,round
        character f1*500,extn*20,fn*500,keyword*500,fg*500,fname*500
        character comment*500,code*1,keystrng*500,dir*500,scratch*500
        real*8 maxv,cdelt(3),bmaj,kappa,rmsclip,bmpersrc,keyvalue 
        integer brightsize,largesize,intersrcsep,error

        if (boxsize.eq.0) then
         extn='.bstat'
         keyword='max_value'
         call get_keyword(fname,extn,keyword,keystrng,maxv,comment,
     /        'r',scratch,error)
         keyword='rms_clip'
         call get_keyword(fname,extn,keyword,keystrng,rmsclip,
     /        comment,'r',scratch,error)
         extn='.bparms'
         keyword='bmpersrc_th'
         call get_keyword(fname,extn,keyword,keystrng,bmpersrc,
     /        comment,'r',scratch,error)
         extn='.header'
         keyword='CDELT1'
         call get_keyword(fname,extn,keyword,keystrng,cdelt(1),
     /        comment,'r',scratch,error)
         keyword='BMAJ'
         call get_keyword(fname,extn,keyword,keystrng,bmaj,
     /        comment,'r',scratch,error)
         kappa=3.d0
         brightsize=round(2.d0*bmaj/abs(cdelt(1))/fwsig*sqrt(2.d0*dlog
     /              (maxv/(kappa*rmsclip))))   ! 'size' of brightest source
         largesize=round(min(n,m)/4.d0)        ! atleast 4 boxes on each side
         intersrcsep=round(sqrt(bmpersrc)*2.d0*bmaj/abs(cdelt(1)))
c!       scales in noise part of map
c!       wavelet decomp
c!       scales in dirty beam
 
         boxsize=round(sqrt(brightsize*largesize*1.d0))
         if (intersrcsep.gt.brightsize.and.intersrcsep.lt.largesize)
     /    boxsize=round(sqrt(intersrcsep*largesize*1.d0))
         if (boxsize.lt.40) then
          boxsize=min(40,min(n,m))
         end if
         boxsize=boxsize-1 ! horrible hack fro WN40305HB, check later 
         if (mod(boxsize,10).eq.0) boxsize=boxsize+1
        end if ! if boxsize=0 in paradefine
c! and so on for each estimate commented above.

        if (stepsize.eq.0) 
     /   stepsize=round(min(boxsize/3.d0,min(n,m)/10.d0))

        extn='.bparms'
        keyword='boxsize_th'
        comment=" 'Boxsize for rms map'"
        call put_keyword(fname,extn,keyword,keyword,boxsize*1.d0,
     /       comment,'R',scratch)
        keyword='stepsize_th'
        comment=" 'Stepsize for rms map'"
        call put_keyword(fname,extn,keyword,keyword,stepsize*1.d0,
     /       comment,'R',scratch)

        return
        end
c!
c! if RA and dec values of top and bottom and left and right resp
c! of vert and hori lines arent the same to within a pixel->arcsec then
c! nonlinear else linear
        subroutine pp_linearcoords(f1,fname,n,m,scratch,imagename,
     /    nn,ctype,crpix,cdelt,crval,crota,bm_pix)
        implicit none
        include "wcs_bdsm.inc"
        include "constants.inc"
        integer nn,n,m,i
        real*8 crpix(nn),cdelt(nn),crval(nn),crota(nn),bm_pix(nn)
        real*8 image(n,m),blankn,blankv
        real*8 blcra,blcdec,tlcdec,tlcra,brcdec,brcra
        character f1*500,ctype(nn)*8,extn*20,scratch*500
        character fname*500,imagename*500
        character keyword*500,fg*500,comment*500,keystrng*500
        integer wcslen,iaxis(2)
        parameter (wcslen=450)
        integer wcs(wcslen),blcx,tlcy,brcx,error,error1
        logical isgoodpix
        character linear*500

c! read in image and blank value and get wcs
        extn='.img'
        call readarray_bin(n,m,image,n,m,imagename,extn)
        call wcs_struct(nn,ctype,crpix,cdelt,crval,crota,wcs,wcslen)

        extn='.bstat'
        keyword='blank_num'
        call get_keyword(fname,extn,keyword,keystrng,blankn,
     /       comment,'r',scratch,error)
        if (blankn.gt.0.d0) then
         extn='.bstat'
         keyword='blank_val'
         call get_keyword(fname,extn,keyword,keystrng,blankv,
     /        comment,'r',scratch,error)
        end if

c! first find nonblank lines inside the universe.
c! ignore if linear but rotated. too difficult to take care of now
        error=0
        do i=1,min(n,m)  ! go diagonally to find them
         call sub_lincoord_gpix(i,i,isgoodpix,wcs,wcslen,
     /        blankn,blankv,image(i,i))
         if (isgoodpix) then
          blcx=i
          goto 443
         end if
        end do
443     continue
        if (blcx.eq.min(n,m)) error=1
        do i=m,blcx,-1
         call sub_lincoord_gpix(blcx,i,isgoodpix,wcs,wcslen,
     /        blankn,blankv,image(blcx,i))
         if (isgoodpix) then
          tlcy=i
          goto 444
         end if
         if (i.eq.blcx) error=error+2
        end do
444     continue
        do i=n,blcx,-1
         call sub_lincoord_gpix(i,blcx,isgoodpix,wcs,wcslen,
     /        blankn,blankv,image(i,blcx))
         if (isgoodpix) then
          brcx=i
          goto 445
         end if
         if (i.eq.blcx) error=error+4
        end do
445     continue
        
c! get coordinates
        if (error.gt.0) then
         write (*,*) ' !!! Cant get proper coords, error ',error
        else
         call wcs_xy2radec(blcx*1.d0,blcx*1.d0,blcra,blcdec,
     /        error1,wcs,wcslen)
         call wcs_xy2radec(blcx*1.d0,tlcy*1.d0,tlcra,tlcdec,
     /        error1,wcs,wcslen)
         call wcs_xy2radec(brcx*1.d0,blcx*1.d0,brcra,brcdec,
     /        error1,wcs,wcslen)  ! coords in radians, cdelt in deg
        end if

c! check if linear
        linear='true'
        iaxis(1)=0
        iaxis(2)=0
        do i=1,nn
         if (ctype(i)(1:2).eq.'RA') iaxis(1)=i
         if (ctype(i)(1:3).eq.'DEC') iaxis(2)=i
        end do
        if (iaxis(1).eq.0.or.iaxis(2).eq.0) then
         write (*,*) ' WRONG AXES !!'
         linear='true'
        else
         if (abs(blcra-tlcra).gt.abs(cdelt(iaxis(1)))/rad) 
     /       linear='false'
         if (abs(blcdec-brcdec).gt.abs(cdelt(iaxis(2)))/rad) 
     /       linear='false'
        end if

        extn='.bparms'
        keyword='linearcoords'
        comment=" 'Assume dirn of north is same for all pixels'"
        call put_keyword(fname,extn,keyword,linear,0.d0,
     /       comment,'S',scratch)
c        call freewcs(wcs,wcslen)

        return
        end
c!
c!
c!
        subroutine sub_lincoord_gpix(i,j,isgoodpix,wcs,
     /             wcslen,blank_num,blankv,imval)
        implicit none
        integer i,j,wcslen,wcs(wcslen)
        logical isgoodpix,isgood1
        real*8 blank_num,blankv,imval

        isgoodpix=.true.
        if (blank_num.gt.0.d0.and.imval.eq.blankv) 
     /   isgoodpix=.false.
        if (isgoodpix) then
         call wcs_isgoodpix(i*1.d0,i*1.d0,wcs,wcslen,isgood1)
         isgoodpix=isgoodpix.and.isgood1
        end if
        
        return  
        end




