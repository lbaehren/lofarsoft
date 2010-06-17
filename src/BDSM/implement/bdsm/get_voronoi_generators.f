c! this gets the list of voronoi generators. right now it is all the
c! centres of brightest individual sources.
c! it can also be imported from metadata in future (from calibration e.g.)
c! ncut is no. of srcs used in statistics above snrcut
c! generators=calibrator/field. calib means only one src per facet
c! and field means all srcs between snrbot and snrtop are secondary and
c! as for calib, between snrtop and snrmax is primary.

        subroutine get_voronoi_generators(scratch,n,m,peak,im_rms,
     /    imagename,fullname,snrcut,snrtop,snrbot,npts,nptss,f_sclip,
     /    ncut,xpix,ypix,generators,gencode,primarygen)
        implicit none
        integer npts,n,m,ncut,i,num,iwksp(ncut),f_sclip(ncut),dumi
        integer cutoff,minnum,nchar,dumi1,round,cutoffs,nptss
        real*8 image(n,m),snrcut,peak(ncut),im_rms(ncut),snr(ncut)
        real*8 xpix(ncut),ypix(ncut),x(ncut),y(ncut),wksp(ncut)
        real*8 maxsnr,minsnr,snrtop,snrbot
        character scratch*500,imagename*500,extn*20,fullname*500,fn*500
        character generators*500,primarygen*500,gencode*4,fn1*500

        extn='.img'
        call readarray_bin(n,m,image,n,m,imagename,extn)

        do i=1,ncut
         snr(i)=peak(i)/im_rms(i)
         x(i)=xpix(i)
         y(i)=ypix(i)
        end do
        call sort3_3(ncut,snr,x,y,wksp,iwksp)

c! ad hoc choose brightest even if flagged by f_sclip cos at high snr 
c! the clipping isnt that reliable for now.
        if (generators(1:nchar(generators)).eq.'calibrators') then
         if (gencode(1:4).ne.'list'.and.gencode(1:4).ne.'file') 
     /       gencode='list'
         if (gencode(1:4).eq.'list') then ! take from gaussian list 
          if (snrtop.eq.0.0) then
           minnum=min(20,ncut)    ! atleast 20 or all sources
           maxsnr=0.1d0*snr(ncut) ! go downto atleast 0.1 snr unless is not in list
           minsnr=20.d0
           if (minsnr.lt.snrcut) minsnr=snrcut
           if (snr(1).gt.maxsnr) maxsnr=snr(1)
           if (maxsnr.lt.minsnr) maxsnr=sqrt(minsnr*snr(ncut))
           do i=ncut,1,-1
            if (snr(i).gt.maxsnr) dumi=i
            if (snr(i).gt.minsnr) dumi1=i
           end do
           cutoff=min(dumi,ncut-minnum)
           if (snr(ncut-minnum).lt.maxsnr) then
            cutoff=ncut-minnum     ! cutoff is index of snr array above which to take
           else
            cutoff=dumi
           end if
           if (snr(cutoff).lt.minsnr) cutoff=dumi1
           npts=ncut-cutoff+1
          else
           cutoff=round(ncut*(1.d0-snrtop)) ! index
           npts=ncut-cutoff+1               ! number of pts
          end if   ! snrtop=0
         end if  ! gencode=list
        end if ! generator=calibrator

        if (generators.eq.'field') then
         cutoff=round(ncut*(1.d0-snrtop))
         npts=ncut-cutoff+1
         cutoffs=round(ncut*(1.d0-snrbot))
         nptss=cutoff-cutoffs
        end if

         write (*,*) 'npts, nptss = ',npts, nptss
c! write to file        
        if (generators.eq.'calibrators') then
         if (gencode.eq.'file') then
          fn1=scratch(1:nchar(scratch))//fullname(1:nchar(fullname))//
     /       primarygen(1:nchar(primarygen))  ! primary source
          npts=0
          open(unit=21,file=fn1)
           read (21,*,END=100) x(npts+1),y(npts+1),snr(npts+1)
           npts=npts+1
          close(21)
100       continue
          if (npts.gt.ncut) write (*,*) ' !! TOO MANY SOURCES ??'
          cutoff=1
         end if
        end if
c! for calibrator+(file/list) and field
        fn=scratch(1:nchar(scratch))//fullname(1:nchar(fullname))//
     /     '.vorogensP'  ! primary sources
        open(unit=21,file=fn)
        write (21,*) npts
        do i=ncut,cutoff,-1
         write (21,*) x(i),y(i),snr(i)
        end do
        close(21)
c! for field
        nptss=0
        if (generators.eq.'field') then
         fn=scratch(1:nchar(scratch))//fullname(1:nchar(fullname))//
     /      '.vorogensS'  ! primary sources
         open(unit=21,file=fn)
         write (21,*) nptss
         do i=cutoff-1,cutoffs,-1
          write (21,*) x(i),y(i),snr(i)
         end do
         close(21)
        end if

        return
        end


