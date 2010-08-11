c! takes image and sourcelist and makes resid image and source image.
c! CHANGE GAUL FORMAT

        subroutine make_src_residim(f2,imagename,runcode,srldir,
     /             n,m,l0,l,gausshap,scratch)
        implicit none
        integer n,m,nsrc,nffmt,nisl,gpi,l,l0,nchar,ndum,mdum
        character f1*500,f2*500,ffmt*500,runcode*2,srldir*500
        character imagename*500,gausshap*500,scratch*500

        if (runcode(2:2).eq.'q') write (*,*) '  Making residual images'
        call sourcelistheaders(f2,f1,ndum,mdum,nisl,nsrc,gpi,nffmt,
     /       ffmt,srldir)  ! gaul headers are written even not fit.
        if (ndum.ne.n.or.mdum.ne.m) write (*,*) '  something fishy'
        call sub_make_src_residim(f1,f2,n,m,nsrc,nffmt,ffmt,runcode,
     /       imagename,gausshap,scratch,srldir,nisl)

        return
        end

c! -----------------------------------------------------------------

        subroutine sub_make_src_residim(f1,f2,n,m,nsrc,nffmt,
     /       ffmt,runcode,imagename,gausshap,scratch,srldir,nisl)
        implicit none
        include "constants.inc"
        integer n,m,nsrc,nffmt,isrc,dumi,nisl,round,i,j,nchar
        character f1*500,fn*500,extn*20,f2*500,ffmt*500,head*100
        real*8 image(n,m),resid(n,m),srcim(n,m),std
        real*8 dumr,magic
        real*8 cdelt(3),keyvalue,dumr1,blankn,blankv
        integer error
        character keyword*500,keystrng*500,comment*500,runcode*2
        character dir*500,fg*500,scratch*500,srldir*500,imagename*500
        character gausshap*500
        logical gaus,shap

        magic=-999.d0

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
        
        extn='.img'
        call readarray_bin(n,m,image,n,m,imagename,extn)

        extn='.header'
        keyword='CDELT1'
        call get_keyword(f2,extn,keyword,keystrng,cdelt(1),
     /       comment,'r',scratch,error)
        keyword='CDELT2'
        call get_keyword(f2,extn,keyword,keystrng,cdelt(2),
     /       comment,'r',scratch,error)

        extn='.bstat'
        keyword='rms_clip'
        call get_keyword(f2,extn,keyword,keystrng,std,
     /       comment,'r',scratch,error)

        if (gaus) then
         call initialiseimage(srcim,n,m,n,m,0.d0)
         call make_resid_gaus(image,srldir,f2,nsrc,cdelt,std,
     /        n,m,srcim,scratch,resid)
         fn=f2(1:nchar(f2))//'.srcim.gaus'
         call writearray_bin2D(srcim,n,m,n,m,fn,runcode)
         fn=f2(1:nchar(f2))//'.resid.gaus'
         call writearray_bin2D(resid,n,m,n,m,fn,runcode)
        end if
        if (shap) then
         call initialiseimage(srcim,n,m,n,m,magic)
         call make_resid_shap(image,srldir,f2,nisl,n,m,
     /        srcim,scratch,resid,magic)
         fn=f2(1:nchar(f2))//'.srcim.shap'
         call writearray_bin2D(srcim,n,m,n,m,fn,runcode)
         fn=f2(1:nchar(f2))//'.resid.shap'
         call writearray_bin2D(resid,n,m,n,m,fn,runcode)
        end if

        return
        end
c!
c!      FOR GAUSSIANS
c!
        subroutine make_resid_gaus(image,srldir,f2,nsrc,cdelt,std,
     /            n,m,srcim,scratch,resid)
        implicit none
        include "constants.inc"
        integer n,m,i,j,nsrc,isrc,nchar,nisl,flag,srcn,error,dumi
        integer round,blc1,blc2,trc1,trc2
        real*8 cdelt(2),std,level,hsize,srcim(n,m),dumr
        real*8 a1,ra,dec,a2,a3,bmaj,bmin,bpa,dbmaj,dbmin,dbpa
        real*8 sstd,sav,rstd,rav,chi,q,image(n,m),resid(n,m)
        real*8 blankn,blankv,a4,a5,realrms
        character scratch*500,f2*500,fn*500,comment*500,keyword*500
        character extn*20,keystrng*500,srldir*50        
        real*8 dumr1,dumr2,dumr3,dumr4,dumr5,dumr6

        level=3.d0  ! go down to sigma/3 to put in source
        fn=srldir(1:nchar(srldir))//f2(1:nchar(f2))//'.gaul.bin'   
        open(unit=22,file=fn(1:nchar(fn)),form='unformatted') 
        do 100 isrc=1,nsrc
         read (22) dumi,nisl,flag
         if (flag.eq.0) then
          backspace(22)
          read (22) dumi,nisl,flag,dumr,dumr,a1,dumr,ra,dumr,dec,dumr
     /     ,a2,dumr,a3,dumr,bmaj,dumr,bmin,dumr,bpa,dumr,
     /     dbmaj,dumr,dbmin,dumr,dbpa,dumr,sstd,sav,rstd,rav,chi,q,srcn,
     /     blc1,blc2,trc1,trc2,realrms,dumr2,dumr3,dumr4,dumr5,dumr6 ! fw_asec
          a4=bmaj/fwsig/abs(cdelt(1)*3600.d0)
          a5=bmin/fwsig/abs(cdelt(2)*3600.d0)
          bpa=(bpa+90.d0)/rad
          hsize=sqrt(-2.d0*dlog(realrms/a1/level))*a4
          do i=max(round(a2-hsize),1),min(round(a2+hsize),n)
           do j=max(round(a3-hsize),1),min(round(a3+hsize),m)
            dumr=a1*dexp(-0.5d0*(
     /       (((i-a2)*dcos(bpa)+(j-a3)*dsin(bpa))/a4)**2.d0+
     /       (((j-a3)*dcos(bpa)-(i-a2)*dsin(bpa))/a5)**2.d0))
            srcim(i,j)=srcim(i,j)+dumr 
           end do
          end do
         end if  ! if flag=0
100     continue
        close(22)

        do j=1,m
         do i=1,n
          resid(i,j)=image(i,j)-srcim(i,j)
         end do
        end do

c! since little of overstep, check for blanks here again
        extn='.bstat'
        keyword='blank_num'
        call get_keyword(f2,extn,keyword,keystrng,blankn,
     /       comment,'r',scratch,error)
        if (blankn.gt.0.d0) then
         extn='.bstat'
         keyword='blank_val'
         call get_keyword(f2,extn,keyword,keystrng,blankv,
     /        comment,'r',scratch,error)
         do j=1,m
          do i=1,n
           if (image(i,j).eq.blankv) resid(i,j)=blankv
          end do
         end do
        end if
        
        return
        end
c!
c!      FOR SHAPELETS
c!
        subroutine make_resid_shap(image,srldir,f2,nisl,n,m,
     /            srcim,scratch,resid,magic)
        implicit none
        include "trhermcoef.inc"
        include "constants.inc" 
        integer nisl,n,m,i,j,iisl,dumi,nmax(nisl),subn,subm
        integer blc(2,nisl),trc(2,nisl),nchar,maxsize,maxmem
        integer maxnmax
        real*8 image(n,m),resid(n,m),beta(nisl),srcim(n,m)
        real*8 xcen(nisl),ycen(nisl)
        real*8 dumr,magic
        character scratch*500,srldir*500,f2*500,fn*500,fn1*500,dumc*500
        
        fn1=scratch(1:nchar(scratch))//f2(1:nchar(f2))//'.islandlist'
        open(unit=25,file=fn1(1:nchar(fn1)),form='unformatted') 
        read (25) dumi,dumi,dumi,maxmem
        close(25)

        fn=srldir(1:nchar(srldir))//f2(1:nchar(f2))//'.shapelet.c'
        open(unit=22,file=fn(1:nchar(fn)),form='formatted') 
        call readhead_srclist(22,19,'Max_nmax_per_island',
     /       dumc,dumi,maxnmax,'i')
333     read (22,*) dumc
        if (dumc.ne.'fmt') goto 333
        maxsize=0
        do iisl=1,nisl
         read (22,*) dumi,nmax(iisl),beta(iisl),xcen(iisl),ycen(iisl),
     /       dumr,dumr,blc(1,iisl),blc(2,iisl),trc(1,iisl),trc(2,iisl)
         if (dumi.ne.iisl) write (*,*) '  shape problem !!!!'
         subn=trc(1,iisl)-blc(1,iisl)+1
         subm=trc(2,iisl)-blc(2,iisl)+1
         maxsize=max(maxsize,subn)
         maxsize=max(maxsize,subm)
        end do
        call sub_make_resid_shap(nmax,hc,nhc,nisl,beta,xcen,ycen,
     /       f2,srldir,maxsize,image,resid,n,m,srcim,blc,trc,maxmem,
     /       scratch,magic)
        close(22)

        return
        end
c!
c!
c! add to srcim cos u can have two islands overlapping (like pt src
c! inside ring).
        subroutine sub_make_resid_shap(nmax,hc,nhc,nisl,beta,xcen,ycen,
     /     f2,srldir,maxsize,image,resid,n,m,srcim,blc,trc,maxmem,
     /     scratch,magic)
        implicit none
        integer nisl,nmax(nisl),nhc,n,m,maxsize,nchar,iisl,nnum,ix,iy
        integer i,j,blc(2,nisl),subn,subm,trc(2,nisl),maxmem,delx,dely
        real*8 hc(nhc,nhc),xcenn,ycenn,magic
        real*8 subim(maxsize,maxsize),beta(nisl),xcen(nisl),ycen(nisl)
        real*8 image(n,m),resid(n,m),srcim(n,m)
        character f2*500,srldir*500,fn*500,fn1*500,scratch*500,f3*500
        integer xpix(maxmem),ypix(maxmem),npix_isl,dumi

        fn1=scratch(1:nchar(scratch))//f2(1:nchar(f2))//'.islandlist'
        open(unit=25,file=fn1(1:nchar(fn1)),form='unformatted') 
        read (25)

        fn=srldir(1:nchar(srldir))//f2(1:nchar(f2))//'.shapcoef.c'
        open(unit=23,file=fn(1:nchar(fn)),form='formatted') 
        do iisl=1,nisl
         read (25) dumi,npix_isl,dumi,dumi
         call sub_readislpix(25,npix_isl,maxmem,xpix,ypix)

         xcenn=1.d0+xcen(iisl)-blc(1,iisl)
         ycenn=1.d0+ycen(iisl)-blc(2,iisl)
         subn=trc(1,iisl)-blc(1,iisl)+1
         subm=trc(2,iisl)-blc(2,iisl)+1
         nnum=nmax(iisl)*(nmax(iisl)-1)/2
         delx=blc(1,iisl)-1
         dely=blc(2,iisl)-1       ! coord_im = coord_subim + (delx,dely)
         call call_recons_cart(nmax(iisl),xcenn,ycenn,subn,subm,
     /     subim,maxsize,23,beta(iisl),nnum,nhc,hc,maxmem,
     /     xpix,ypix,npix_isl,delx,dely)

         do i=1,subn
          do j=1,subm
           ix=i-1+blc(1,iisl)
           iy=j-1+blc(2,iisl)
           if (srcim(ix,iy).eq.magic) then 
            srcim(ix,iy)=subim(i,j)
            if (subim(i,j).ne.magic) then
             resid(ix,iy)=image(ix,iy)-subim(i,j)
            else
             resid(ix,iy)=image(ix,iy)
            end if
           end if
          end do
         end do
        
        end do
        close(23)
        close(25)

        return
        end
c!
c!
c!
        subroutine call_recons_cart(nmaxx,xcen,ycen,subn,subm,
     /       subim,maxsize,nn,bet,nnum,nhc,hc,maxmem,xpix,ypix,
     /       npix_isl,delx,dely)
        implicit none
        integer nmaxx,subn,subm,maxsize,nnum,i,j,nn,nhc,dumi
        integer mask(subn,subm),maxmem,xpix(maxmem),ypix(maxmem)
        integer npix_isl,delx,dely
        real*8 subim(maxsize,maxsize),cfvec(nnum)
        real*8 xcen,ycen,cf(nmaxx,nmaxx),bet
        real*8 hc(nhc,nhc),image(subn,subm)
        
        call initialiseimage(cf,nmaxx,nmaxx,nmaxx,nmaxx,0.d0)
        call initialisemask(mask,subn,subm,subn,subm,0)
        do i=1,npix_isl
         mask(xpix(i)-delx,ypix(i)-dely)=1
        end do

        read (nn,*) dumi,cfvec 
        dumi=0
        do i=1,nmaxx
         do j=1,nmaxx-i
          dumi=dumi+1
          cf(i,j)=cfvec(dumi)
         end do
        end do
        
        call recons_cart(subn,subm,hc,bet,xcen,ycen,nmaxx,cf,
     /       image,nhc)
        do i=1,subn
         do j=1,subm
          subim(i,j)=image(i,j)
          if (mask(i,j).eq.0) subim(i,j)=0.d0
         end do
        end do

        return
        end


