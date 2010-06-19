c! calculate spectral index. first attempt
c! CHANGE GAUL FORMAT
c! CHANGE SRL FORMAT

        subroutine callcalcspin(filename,srldir,imagename,scratch,
     /        fullname,n1,m1,l0,l)
        implicit none
        character filename*500,srldir*500,imagename*500,scratch*500
        character fullname*500,f1*500,fn*500,ffmt*500,dumc*500
        character hdrfile*500,ctype(3)*8
        integer n,m,l,n1,m1,nisl,ngau,nsrc,gpi,nffmt,nisl1,nchar,l0
        integer maxmem,n_s,m_s,l_s
        real*8 dumr,crpix(3),cdelt(3),crval(3),crota(3),bm_pix(3)

        call sourcelistheaders(fullname,f1,n,m,nisl,ngau,gpi,nffmt,
     /       ffmt,srldir)
        if (n.ne.n1.or.m.ne.m1) write (*,*) ' @@  EERROORRR'
        fn=srldir(1:nchar(srldir))//fullname(1:nchar(fullname))//'.srl'
        open(unit=22,file=fn(1:nchar(fn)),form='formatted') 
        call readhead_srclist(22,17,'Number_of_sources',dumc,nsrc,
     /       dumr,'i')
        close(22)

        if (l0.gt.1) then
         write (*,*) '  Calculating spectral indices'
         fn=scratch(1:nchar(scratch))//fullname(1:nchar(fullname))//
     /      '.islandlist'
         open(unit=21,file=fn(1:nchar(fn)),form='unformatted') 
         read (21) n,m,nisl1,maxmem
         if (nisl.ne.nisl1) write (*,*) ' ((ERRORR))'

         hdrfile=fullname(1:nchar(fullname))//'.header'
         call read_head_coord(hdrfile,3,ctype,crpix,cdelt,
     /         crval,crota,bm_pix,scratch)
         if (ctype(3)(1:4).ne.'FREQ') write (*,*) ' 3rd axis not okay' 

         n_s=min(bm_pix(1)*10,n)
         m_s=min(bm_pix(2)*10,m)
         l_s=l0
         call calcspin1(filename,srldir,imagename,scratch,
     /      fullname,n,m,l0,l,nisl,ngau,nsrc,gpi,maxmem,ctype,
     /      crpix,cdelt,crval,crota,bm_pix,n_s,m_s,l_s)
         close(21)
        end if

        return
        end
c!
c!
c! This assumes all channels have same restoring and dirty beams so fit cylinder at each gaussian.
c! will fit spectral indices to each pixel in islandlist.
c! actually, no ... each image wont be exactly coincident due to ionosphere ...

        subroutine calcspin1(filename,srldir,imagename,scratch,
     /      fullname,n1,m1,l0,l,nisl,ngau,nsrc,gpi,maxmem,ctype,
     /      crpix,cdelt,crval,crota,bm_pix,n_s,m_s,l_s)
        implicit none
        include "constants.inc"
        character filename*500,srldir*500,imagename*500,scratch*500
        character fullname*500,f1*500,fn*500,ffmt*500,extn*20,f2*500
        character keyword*500,comment*500,keystrng*500
        character hdrfile,ctype(3)*8
        real*8 crpix(3),cdelt(3),crval(3),crota(3),bm_pix(3)
        integer n,m,l,n1,m1,nisl,ngau,nsrc,gpi,nffmt,i,ipix,j,k,l0
        integer maxmem,xpixs(maxmem),ypixs(maxmem),nchar,npix_isl
        integer dumi,k0,ii
        integer n_s,m_s,l_s
        real*8 cube(n1,m1,l0),spec(l0),keyvalue,scube(n_s,m_s,l_s)
        real*8 freq(l0),dumr,stdclip(l0),rmsim(n1,m1),stdclipch0,sig(l0)
        real*8 I0,spin0,spin1,avclip(l0),avclipch0,avim(n1,m1),std0
        integer gauln(ngau),isl(ngau),flag(ngau),srcnum(ngau)
        integer blc1(ngau),blc2(ngau),trc1(ngau),trc2(ngau)
        real*8 totfl(ngau),etotfl(ngau),peakfl(ngau),epeakfl(ngau)
        real*8 ra(ngau),era(ngau),dec(ngau)
        real*8 edec(ngau),xpix(ngau),expix(ngau),ypix(ngau),eypix(ngau)
        real*8 bmaj(ngau),ebmaj(ngau),bmin(ngau),ebmin(ngau),bpa(ngau)
        real*8 ebpa(ngau),dbmaj(ngau),edbmaj(ngau),dbmin(ngau)
        real*8 edbmin(ngau),dbpa(ngau),edbpa(ngau),rmssrc(ngau)
        real*8 avsrc(ngau),rmsisl(ngau),avisl(ngau),spin(ngau)
        real*8 espin(ngau),mnstd,mxstd,dumr9(9),srcpeak(nsrc)
        real*8 srcdec(nsrc),bminsrc(nsrc),srcra(nsrc),peak1ch
        real*8 bmajsrc(nsrc),diffpix,minpix_isl
        real*8 srcx(nsrc),srcy(nsrc),thresh_sp1,thresh_sp2,stdav,avav
        real*8 thresh_sc1,mm1,mm2(2),mm3(2),thresh_1more
        real*8 dumr1(ngau),dumr2(ngau),dumr3(ngau),dumr4(ngau)
        real*8 dumr5(ngau),dumr6(ngau)
        character code(nsrc)*4
        integer wcslen,error,delx,dely,delz
        real*8 dumi1,dumi2
        parameter (wcslen=450)
        integer wcs(wcslen)

        write (*,*) ' *&@$#  spin thresh @!(#$^(@^$#@$'
        thresh_sp1=5.d0
        thresh_sp2=5.d0
        thresh_sc1=5.d0

        call wcs_struct(3,ctype,crpix,cdelt,crval,crota,wcs,wcslen)
        extn='.bstat'
        keyword='rms_clip'
        call get_keyword(fullname,extn,keyword,keystrng,stdclipch0,
     /       comment,'r',scratch,error)
        keyword='mean_clip'
        call get_keyword(fullname,extn,keyword,keystrng,avclipch0,
     /       comment,'r',scratch,error)
        extn='.bparms'
        keyword='mean_map'
        call get_keyword(fullname,extn,keyword,mean_map,dumr,
     /       comment,'s',scratch,error)
        if (mean_map.eq.'false') avclipch0=0.d0
        keyword="minpix_isl"
        call get_keyword(fullname,extn,keyword,keystrng,minpix_isl,
     /    comment,"r",scratch,error)

c! get image 
        extn='.img'
        call readarray_bin3D(n1,m1,l0,cube,n1,m1,l0,filename,extn)

c! get rms and mean image
c        call get_rmsmap(fullname,n1,m1,rmsim)
c        call get_avmap(fullname,n1,m1,avim)
        call get_rmsmap(fullname,n,m,rmsim)
        call get_avmap(fullname,n,m,avim)

c! get rms vs channel from .qc_cc file
        fn=scratch(1:nchar(scratch))//filename(1:nchar(filename))//
     /     '.qc_cc'
        open(unit=21,file=fn,form='unformatted') 
         read (21)
         do k=1,l0
          read (21) dumr,dumi,dumi,dumr,dumi,dumi,dumi,dumr,
     /              dumr,stdclip(k),avclip(k)
         end do
        close(21)

c! calculate an rms per channel to use for thresholding
        if (l0.le.4) then
         call rangevec8(stdclip,l0,l0,mnstd,mxstd)
         std0=mxstd           ! in case combine very different bands together with diff rms
        else
         call calcmedian(stdclip,l0,1,l0,std0)
        end if

c! one spin for every 'source' so read in gaul and srl first
        f2=srldir(1:nchar(srldir))//fullname(1:nchar(fullname))//
     /     '.gaul.bin'
        open(unit=21,file=f2,form='unformatted')
        do i=1,ngau
         read (21) gauln(i),isl(i),flag(i),totfl(i),etotfl(i),
     /     peakfl(i),epeakfl(i),ra(i),era(i),dec(i),
     /     edec(i),xpix(i),expix(i),ypix(i),eypix(i),
     /     bmaj(i),ebmaj(i),bmin(i),ebmin(i),bpa(i),
     /     ebpa(i),dbmaj(i),edbmaj(i),dbmin(i),edbmin(i),
     /     dbpa(i),edbpa(i),rmssrc(i),avsrc(i),rmsisl(i),
     /     avisl(i),spin(i),espin(i),srcnum(i),blc1(i),blc2(i),
     /     trc1(i),trc2(i),dumr1(i),dumr2(i),dumr3(i),dumr4(i)
     /     dumr5(i),dumr6(i)
        end do
        close(21)
        f2=srldir(1:nchar(srldir))//fullname(1:nchar(fullname))//
     /     '.srl.bin'
        open(unit=21,file=f2,form='unformatted')
        do i=1,nsrc
         read (21) dumi,dumi,dumi,code(i),dumr,dumr,dumr,dumr,
     /    srcpeak(i),
     /    dumr,dumr,dumr,dumr,dumr,srcra(i),dumr,srcdec(i),dumr,
     /    bmajsrc(i),dumr,bminsrc(i)
          call wcs_radec2xy(srcra(i)/rad,srcdec(i)/rad,srcx(i),srcy(i),
     /         error,wcs,wcslen) 
        end do
        close(21)

c! for each source, decide about binning - based on faintest gaussian ?
c! no, first do single gaussian. pass on till 2 sigma of gaussian for ch0 image
c! and in ..._single you can mask out for new threshold.
        do i=1,nsrc
         if (code(i).eq.'S'.or.code(i).eq.'C') then    ! for point sources first
          if (srcpeak(i)/std0.ge.thresh_sp1) then
           dumr=bminsrc(i)/fwsig/(abs(cdelt(2))*3600.d0) ! bmin in sigma-pixels
           peak1ch=srcpeak(i)*dexp(-0.5d0/(dumr*dumr)) ! intensity one pixel away
           if (peak1ch/std0.ge.thresh_sp2) then
            stdav=0.d0
            avav=0.d0
            dumi=0
            do j=int(-bm_pix(2)*1.5),int(bm_pix(2)*1.5)  ! 1.5 is a hack
             do ii=int(-bm_pix(1)*1.5),int(bm_pix(1)*1.5)  ! 1.5 is a hack
               stdav=stdav+rmsim(int(srcx(i))+ii,int(srcy(i))+j)
               avav=avav+avim(int(srcx(i))+ii,int(srcy(i))+j)
               dumi=dumi+1
             end do             
            end do
            stdav=stdav/dumi                                         ! stdclip = rms(nu)
            avav=avav/dumi                                         ! stdclip = rms(nu)

            thresh_1more=2.d0                ! down to 2 sigma of ch0
            dumr=bmajsrc(i)/fwsig/(abs(cdelt(1))*3600.d0) ! bmin in sigma-pixels
            diffpix=sqrt(dlog(srcpeak(i)/thresh_1more/stdav)*2.d0*dumr)

            delx=max(1,int(srcx(i)-diffpix))
            dely=max(1,int(srcy(i)-diffpix))
            delz=0
            dumi1=min(n,int(srcx(i)+diffpix))-delx
            dumi2=min(m,int(srcy(i)+diffpix))-dely
            do k=1,l_s
             do j=1,dumi2
              do ii=1,dumi1
               scube(ii,j,k)=cube(ii+delx,j+dely,k+delz) 
              end do             
             end do             
            end do
            call calcspin_single(scube,n_s,m_s,l_s,stdav,avav, ! stdav = average rms
     /        stdclipch0,avclipch0,stdclip,avclip,thresh_sc1,mm1,mm2,
     /        mm3,delx,dely,dumi1,dumi2,minpix_isl,crval,cdelt,crpix,
     /        mean_map)

           else      ! snr of 1 pixel away < thresh
           end if
          else       !  snr of peak < thresh
          end if
         end if
        end do

! bin if necessary

        call freewcs(wcs,wcslen)







c! this is to fit, later....

c        fn=scratch(1:nchar(scratch))//fullname(1:nchar(fullname))//
c     /     '.islandlist'
c        open(unit=21,file=fn(1:nchar(fn)),form='unformatted') 
c        read (21) 
c        do i=1,nisl
c         read (21) j,npix_isl
c         if (j.ne.i) write (*,*) '  ##$$ ERRORR $$##'
c         call sub_readislpix(21,npix_isl,maxmem,xpixs,ypixs)
c         do ipix=1,npix_isl  ! take each pixel in island i and
c          k0=0
c          do k=1,l0           ! fit to spectrum
c           dumr=cube(xpixs(ipix),ypixs(ipix),k)
c           if (dumr.ne.-999.d0) then
c            k0=k0+1
c            spec(k0)=dumr
c            freq(k0)=crval(3)+cdelt(3)*(k-crpix(3))
c            sig(k0)=stdclip(k)*rmsim(xpixs(ipix),ypixs(ipix))/stdclipch0
c         if (xpixs(ipix).eq.493.and.ypixs(ipix).eq.548) write (*,*)
c     /      spec(k0),freq(k0),sig(k0),stdclip(k)
c           end if
c          end do
c         if (xpixs(ipix).eq.493.and.ypixs(ipix).eq.548) then
c          if (k0.gt.1) call fitspix(l0,k0,spec,freq,sig,crval(3),I0,
c     /           spin0,spin1) !logI=logI0+spin0*log(f/f0)+spin1*log^2(f/f0)
c         end if
c         end do
c        end do
c        close(21)


        return
        end
c!
c!
c!
        subroutine calcspin_single(scube,n,m,l,stdav,avav,
     /      stdclipch0,avclipch0,stdclip,avclip,thresh_sc1,m1,m2,m3,
     /      delx,dely,sizex,sizey,minpix_isl,crval,cdelt,crpix,
     /      mean_map)
        implicit none
        integer n,m,l,i,j,k,mask(n,m),delx,dely,fmask(l),dumi
        real*8 scube(n,m,l),plane(n,m),rms,mean,stdav,avav,stdclipch0
        real*8 avclipch0,stdclip(l),avclip(l),thresh_sc1,minpix_isl
        real*8 m1,m2(2),m3(3),mm1(l),mm2(2,l),mm3(2,l)
        real*8 sizex,sizey
        real*8 sigma(l),f0,I0,spin0,crval(3),cdelt(3),crpix(3),freq(l)
        character mean_map*500

        do k=1,l
         freq(k)=crval(3)+cdelt(3)*(k-crpix(3))
         rms=stdav/stdclipch0*stdclip(k)
         if (mean_map.eq.'const'.or.mean_map.eq.'map') then
          mean=avav/avclipch0*avclip(k)
         else
          mean=0.d0
         end if
         dumi=0
         do j=1,m
          do i=1,n
           plane(i,j)=scube(i,j,k)
           if (j.gt.sizey.or.i.gt.sizex) then
            mask(i,j)=0
           else
            if (plane(i,j)-mean.gt.thresh_sc1*rms) then
             mask(i,j)=1
             dumi=dumi+1
            else
             mask(i,j)=0
            end if
           end if
          end do
         end do
         if (dumi.lt.int(minpix_isl)) then
          fmask(k)=0
         else
          fmask(k)=1
         end if
         if (fmask(k).eq.1) 
     /       call momentmaskonly(plane,mask,n,m,n,m,m1,m2,m3)
         mm1(k)=m1
         mm2(1,k)=m2(1)+delx
         mm2(2,k)=m2(2)+dely
         mm3(1,k)=m3(1)
         mm3(2,k)=m3(2)
         sigma(k)=rms
        end do

        f0=crval(3)+(crpix(3)-1.d0)*cdelt(3)
        call sub_calcspin_qc_3dsrc(l,freq,fmask,sigma,mm1,mm2,mm3,
     /       f0,I0,spin0)
        
        return
        end
c!
c!
c!
        subroutine sub_calcspin_qc_3dsrc(l,freq,fmask,sigma,mm1,mm2,
     /             mm3,f0,I0,spin0)
        implicit none
        integer l,k,fmask(l)
        real*8 mm1(l),mm2(2,l),mm3(2,l),sigma(l),freq(l),f0,I0,spin0

        call fitspix(l,mm1,freq,fmask,f0,sigma,I0,spin0)
        
        return
        end




