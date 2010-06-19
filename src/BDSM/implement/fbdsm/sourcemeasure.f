c! first attempt at fitting etc. takes each island, finds multiple sources within
c! by some simple algo, fits gaussians. first try, low funda code.
c! bdsm ----> blob detection and source measurement
c!
c! I call a fitting routine from here, which need not be gaussians. Can also
c! call shapelet routine. 
c!
        subroutine sourcemeasure(f1,f2,srldir,imagename,runcode,
     /             scratch,n,m,l0,l)
        implicit none
        character f1*500,fn*500,extn*20,f2*(*),ch1*1,runcode*2
        character srldir*500,scratch*500,writeout*500,imagename*500
        character keyword*500,gausshap*500,comment*500
        integer nchar,n,m,nisl,maxmem,n1,m1,l,l0,error
        real*8 dumr
        logical gaus,shap

        extn='.bparms'
        keyword='gausshap'
        call get_keyword(f2,extn,keyword,gausshap,dumr,
     /       comment,'s',scratch,error)
        if (gausshap(1:1).eq.'1') then
         gaus=.true.
        else
         gaus=.false.
        end if
        if (gausshap(2:2).eq.'1') then
         shap=.true.
        else
         shap=.false.
        end if
        if (gausshap.eq.'00') then
         write (*,*) '  Nothing to fit. Change gausshap in paradefine'
         stop
        end if

c        write (*,*) '  # #$^@# deconv,f is incomplete'
        if (runcode(2:2).eq.'q') then
         if (gaus) then 
          write (*,*) '  Fitting islands with gaussians'
         else
          write (*,*) '  Fitting islands with shapelets'
         end if
         if (gaus.and.shap) write (*,*) '  and shapelets'
        end if
        if (l.gt.1) write (*,*) '  Using 2d array for 3d image !!!'
        
        fn=scratch(1:nchar(scratch))//f2(1:nchar(f2))//'.islandlist'
        open(unit=21,file=fn(1:nchar(fn)),form='unformatted') 
        read (21) n1,m1,nisl,maxmem
        if (n1.ne.n.or.m1.ne.m) write (*,*) '   #### Mismatch !!!! '
        close(21)

        call sub_sourcemeasure(f1,f2,n,m,nisl,maxmem,scratch,
     /       imagename,srldir,runcode,gaus,shap)
c! next step
        if (runcode(1:1).eq.'m') then
         write (*,*) 
         write (*,'(a,$)') '   Continue (residual image : resi) (y)/n ?'
         read (*,'(a1)') ch1
         if (ichar(ch1).eq.32) ch1='y'
333      continue
         if (ch1.eq.'y') call make_src_residim(f2,runcode,srldir)
        end if

        return  
        end

c! ---------------------------------------------------------------------------

c! blc,trc is 1 pixel out from actual island pixels. take these unmasked if < 3sig
        subroutine sub_sourcemeasure(f1,f2,n,m,nisl,maxmem,scratch,
     /             imagename,srldir,runcode,gaus,shap)
        implicit none
        character f1*(*),fn*500,extn*20,f2*(*),scratch*500,imagename*500
        character srldir*500,runcode*2
        integer nchar,n,m,nisl,maxmem,subsize(2)
        logical gaus,shap

        fn=scratch(1:nchar(scratch))//f2(1:nchar(f2))//'.islandlist'
        open(unit=23,file=fn(1:nchar(fn)),form='unformatted') 
        read (23) n,m,nisl,maxmem
        call getmax_subsize(23,nisl,maxmem,n,m,subsize) ! avoid pass full im to fit_ each time
        close(23)

        call sub_sub_sourcemeasure(f1,f2,n,m,nisl,maxmem,subsize,
     /       imagename,scratch,srldir,runcode,gaus,shap)

        return
        end
c!
c!
        subroutine sub_sub_sourcemeasure(f1,f2,n,m,nisl,maxmem,subsize,
     /             imagename,scratch,srldir,runcode,gaus,shap)
        implicit none
        include "wcs_bdsm.inc"
        integer maxs,maxs6
        parameter (maxs=400)
        parameter (maxs6=400*6)
        character f1*(*),fn*500,extn*20,f2*(*),scratch*500,fn1*500
        integer nchar,n,m,nisl,maxmem,i,j,npix_isl,nsrc,subsize(2)
        integer xpix(maxmem),ypix(maxmem),blc(2),trc(2),subn,subm
        integer delx,dely,iisl,gauperisl,error,nsrcini,maxnmax
        real*8 image(n,m),std,rstd,rav,rmsim(n,m),dumr,avim(n,m)
        real*8 submaxim(subsize(1),subsize(2)),thresh_pix
        real*8 submaxav(subsize(1),subsize(2))
        real*8 submaxrms(subsize(1),subsize(2)),keyvalue,minpix_isl
        character dums*100,ffmt*500,fn3*500,fn4*500,runcode*2,fn5*500
        character fn6*500,iniguess*500,flagsmallsrc*500
        logical gaus,shap
        real*8 crpix(3),cdelt(3),crval(3),crota(3),bm_pix(3),avclip
        real*8 blankn,blankv,maxsize_beam
        real*8 bmaj,bmin,bpa,fitfreely
        character ctype(3)*8,prog*20,dir*500,fn2*500,imagename*500
        logical exists
        character comment*500,keyword*500,keystrng*500,f3*500,fg*500
        character srldir*500,rmsmap*500,mean_map*500,linear*500
        integer wcslen,xmax,ymax
        parameter (wcslen=450)
        integer wcs(wcslen)

        prog='bdsm.f'

        extn='.img'
        call readarray_bin(n,m,image,n,m,imagename,extn)
        extn='.bstat'
        keyword='rms_clip'
        call get_keyword(f2,extn,keyword,keystrng,std,
     /       comment,'r',scratch,error)
        extn='.bparms'
        keyword='thresh_pix'
        call get_keyword(f2,extn,keyword,keystrng,thresh_pix,
     /       comment,'r',scratch,error)
        keyword='minpix_isl'
        call get_keyword(f2,extn,keyword,keystrng,minpix_isl,
     /       comment,'r',scratch,error)
        keyword='rms_map'
        call get_keyword(f2,extn,keyword,rmsmap,keyvalue,
     /       comment,'s',scratch,error)
        keyword='linearcoords'
        call get_keyword(f2,extn,keyword,linear,dumr,
     /       comment,'s',scratch,error)
        keyword='maxsize_beam'
        call get_keyword(f2,extn,keyword,keystrng,maxsize_beam,
     /       comment,'r',scratch,error)
        extn='.bstat'
        keyword='blank_num'
        call get_keyword(f2,extn,keyword,keystrng,blankn,
     /       comment,'r',scratch,error)
        if (blankn.gt.0.d0) then
         keyword='blank_val'
         call get_keyword(f2,extn,keyword,keystrng,blankv,
     /        comment,'r',scratch,error)
        end if
        extn='.bparms'
        keyword='fitfreely'
        call get_keyword(f2,extn,keyword,keystrng,fitfreely,
     /       comment,'r',scratch,error)
        keyword='iniguess'
        call get_keyword(f2,extn,keyword,iniguess,dumr,
     /       comment,'s',scratch,error)
        keyword='flagsmallsrc'
        call get_keyword(f2,extn,keyword,flagsmallsrc,dumr,
     /       comment,'s',scratch,error)

        fn=scratch(1:nchar(scratch))//f2(1:nchar(f2))//'.islandlist'
        open(unit=25,file=fn(1:nchar(fn)),form='unformatted') 
        read (25) 

        fn=srldir(1:nchar(srldir))//f2(1:nchar(f2))//'.gaul'    
        fn1=srldir(1:nchar(srldir))//f2(1:nchar(f2))//'.gaul.bin' 
        fn2=scratch(1:nchar(scratch))//f2(1:nchar(f2))//'.scratch' 
        fn3=srldir(1:nchar(srldir))//f2(1:nchar(f2))//'.ini.gaul'  
        fn4=srldir(1:nchar(srldir))//f2(1:nchar(f2))//'.ini.gaul.bin' 
        fn5=srldir(1:nchar(srldir))//f2(1:nchar(f2))//'.shapelet.c' 
        fn6=srldir(1:nchar(srldir))//f2(1:nchar(f2))//'.shapcoef.c'
        call system('rm -f '//fn(1:nchar(fn)))
        call system('rm -f '//fn1(1:nchar(fn1)))
        call system('rm -f '//fn2(1:nchar(fn2)))
        call system('rm -f '//fn3(1:nchar(fn3)))
        call system('rm -f '//fn4(1:nchar(fn4)))
        call system('rm -f '//fn5(1:nchar(fn5)))
        call system('rm -f '//fn6(1:nchar(fn6)))
        open(unit=26,file=fn(1:nchar(fn)),form='formatted') 
        open(unit=27,file=fn1(1:nchar(fn1)),form='unformatted') 
        open(unit=28,file=fn2(1:nchar(fn2)),form='formatted') 
        open(unit=29,file=fn3(1:nchar(fn3)),form='formatted') 
        open(unit=30,file=fn4(1:nchar(fn4)),form='unformatted') 
        open(unit=31,file=fn5(1:nchar(fn5)),form='formatted') 
        open(unit=32,file=fn6(1:nchar(fn6)),form='formatted') 
        call sub_sourcemeasure_writehead(26,f1,f2,n,m,nisl,std,
     /       prog,rmsmap,thresh_pix)
        call sub_sourcemeasure_writehead(29,f1,f2,n,m,nisl,std,
     /       prog,rmsmap,thresh_pix)
        call sub_sourcemeasure_writeheadsh(31,f1,f2,n,m,nisl,std,
     /       prog,rmsmap,thresh_pix)
c!							get headers for pixel-->coord
        fn=scratch(1:nchar(scratch))//f2(1:nchar(f2))//'.header'
        inquire(file=fn,exist=exists)
        call read_dum_head(3,ctype,crpix,cdelt,crval,crota,bm_pix)
        f3=f2(1:nchar(f2))//'.header'
        if (exists) call read_head_coord(f3(1:nchar(f3)),3,
     /     ctype,crpix,cdelt,crval,crota,bm_pix,scratch)
        extn='.header'
        keyword='BMAJ'
        call get_keyword(f2,extn,keyword,keystrng,bmaj,
     /       comment,'r',scratch,error)
        keyword='BMIN'
        call get_keyword(f2,extn,keyword,keystrng,bmin,
     /       comment,'r',scratch,error)
        keyword='BPA'
        call get_keyword(f2,extn,keyword,keystrng,bpa,
     /       comment,'r',scratch,error)

c! load in the rms map
        call get_rmsmap(f2,n,m,rmsim)
        call get_avmap(f2,n,m,avim)

c! prepare WCS struct for wcslib to pass on.
        call wcs_struct(3,ctype,crpix,cdelt,crval,crota,wcs,wcslen)

        nsrc=0
        nsrcini=0
        gauperisl=0
        maxnmax=0
        do 100 iisl=1,nisl
         read (25) j,npix_isl,xmax,ymax
         call sub_readislpix(25,npix_isl,maxmem,xpix,ypix)
         call sub_sourcemeasure_getblctrc(maxmem,npix_isl,xpix,ypix,
     /        blc,trc,n,m)
         subn=trc(1)-blc(1)+1
         subm=trc(2)-blc(2)+1

c! now put subimage in submaxim and pass on
         delx=blc(1)-1
         dely=blc(2)-1       ! coord_im = coord_subim + (delx,dely)
c! read in image and corresponding rms map
         do i=1,subn
          do j=1,subm
           submaxim(i,j)=image(i+delx,j+dely)
           submaxrms(i,j)=rmsim(i+delx,j+dely)
           submaxav(i,j)=avim(i+delx,j+dely)
          end do
         end do
         if (iisl.ne.1) write (28,*)
         write (28,'(5(i6,1x),$)') iisl,blc,trc
         call matav(submaxav,subsize(1),subsize(2),subn,subm,avclip)

         call call_fit_island(submaxim,submaxrms,subsize,f1,xpix,ypix,
     /    npix_isl,maxmem,blc,trc,avclip,subn,subm,rstd,rav,maxs,maxs6,
     /    26,27,28,29,30,31,32,iisl,nsrc,nsrcini,ffmt,ctype,crpix,cdelt,
     /    crval,crota,bm_pix,
     /    delx,dely,gauperisl,scratch,thresh_pix,avclip,minpix_isl,
     /    wcs,wcslen,blankn,blankv,maxsize_beam,linear,
     /    bmaj,bmin,bpa,xmax,ymax,runcode,gaus,shap,maxnmax,fitfreely,
     /    iniguess,flagsmallsrc)

100     continue
        do i=25,32
         close(i)
        end do
        if (.not.gaus) then
         gauperisl=nisl
         nsrc=nisl
        end if
        fn=srldir(1:nchar(srldir))//f2(1:nchar(f2))//'.gaul'    ! sourcelist==srl
        fn1=srldir(1:nchar(srldir))//f2(1:nchar(f2))//'.shapelet.c'    ! sourcelist==srl
        call sub_sourcemeasure_writeheadmore(nsrc,gauperisl,fn,ffmt,
     /       scratch)
        call sub_sourcemeasure_writeheadmoresh(nsrc,maxnmax,fn1,ffmt,
     /       scratch)
        fn=srldir(1:nchar(srldir))//f2(1:nchar(f2))//'.ini.gaul'    ! sourcelist==srl
        call sub_sourcemeasure_writeheadmore(nsrcini,gauperisl,fn,ffmt,
     /       scratch)

c        call freewcs(wcs,wcslen)

        return  
        end
c!
c!
c!
        subroutine call_fit_island(submaxim,submaxrms,subsize,f1,
     /     xpix,ypix,npix_isl,maxmem,blc,trc,av,subn,subm,rstd,
     /     rav,maxs,maxs6,nn,nnb,nnc,nnd,nne,nnf,nng,iisl,nsrc,nsrcini,
     /     ffmt,ctype,crpix,cdelt,crval,crota,bm_pix,delx,dely,
     /     gauperisl,scratch,thresh_pix,avclip,minpix_isl,wcs,
     /     wcslen,blankn,blankv,maxsize_beam,linear,bmaj,bmin,bpa,
     /     xmax,ymax,runcode,gaus,shap,maxnmax,fitfreely,iniguess,
     /     flagsmallsrc)
        implicit none
        integer maxs,maxs6,nn,gauperisl,nnc,nnf,nng
        character f1*(*),fn*500,extn*20,runcode*2
        integer nchar,n,m,nisl,maxmem,i,j,npix_isl,nsrc,subsize(2)
        integer xpix(maxmem),ypix(maxmem),blc(2),trc(2),subn,subm
        integer delx,dely,iisl,nnb,wcslen,wcs(wcslen),dumi,maxnmax
        integer submask(subn,subm),nsrcini,nnd,nne,xmax,ymax
        real*8 std,av,rstd,rav,subim(subn,subm),subrms(subn,subm)
        real*8 submaxim(subsize(1),subsize(2)),avclip,minpix_isl
        real*8 submaxrms(subsize(1),subsize(2)),thresh_pix,blankn
        character dums*100,ffmt*500,iniguess*500,flagsmallsrc*500
        real*8 crpix(3),cdelt(3),crval(3),crota(3),bm_pix(3),blankv
        real*8 maxsize_beam,fitfreely
        character ctype(3)*8,prog*20,scratch*500,linear*500
        logical exists,blankneed,gaus,shap
        real*8 bmaj,bmin,bpa

        do i=1,subn
         do j=1,subm
          subim(i,j)=submaxim(i,j)
          subrms(i,j)=submaxrms(i,j)
         end do
        end do
        dumi=0
        if (blankn.gt.0.d0) then
         do i=1,subn
          do j=1,subm
           if (subrms(i,j).eq.blankv) then
            submask(i,j)=0
            dumi=dumi+1
           else
            submask(i,j)=1
           end if
          end do
         end do
        else
         blankneed=.false.
        end if
        if (dumi.gt.0) then
         blankneed=.true.
        else
         blankneed=.false.
        end if

        call fit_island(subim,subrms,f1,xpix,ypix,npix_isl,maxmem,
     /   blc,trc,av,subn,subm,rstd,rav,maxs,maxs6,nn,nnb,nnc,nnd,
     /   nne,nnf,nng,iisl,nsrc,nsrcini,ffmt,ctype,crpix,cdelt,
     /   crval,crota,bm_pix,delx,dely,
     /   gauperisl,scratch,thresh_pix,avclip,minpix_isl,wcs,wcslen,
     /   blankn,blankv,blankneed,submask,maxsize_beam,linear,
     /   bmaj,bmin,bpa,xmax,ymax,runcode,gaus,shap,maxnmax,
     /   fitfreely,iniguess,flagsmallsrc)

        return  
        end
c!
c! avoid passing full image to fit_island each time, saving time.
        subroutine getmax_subsize(nn,nisl,maxmem,n,m,subsize) 
        implicit none
        integer nn,nisl,maxmem,n,m,subsize(2),i,j,npix_isl
        integer xpix(maxmem),ypix(maxmem)
        integer blc(2),trc(2),subn,subm

        subsize(1)=0
        subsize(2)=0
        do 100 i=1,nisl
         read (nn) j,npix_isl
         call sub_readislpix(nn,npix_isl,maxmem,xpix,ypix)
         call sub_sourcemeasure_getblctrc(maxmem,npix_isl,xpix,ypix,
     /        blc,trc,n,m)
         subn=trc(1)-blc(1)+1
         subm=trc(2)-blc(2)+1
         if (subn.gt.subsize(1)) subsize(1)=subn
         if (subm.gt.subsize(2)) subsize(2)=subm
100     continue

        return
        end

        


