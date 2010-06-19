c! calculate background noise map

        subroutine callrmsd(f1,f2,imagename,runcode,scratch,n,m,l0,l)
        implicit none
        character f1*500,extn*20,ch1*1,runcode*2,f2*500
        character scratch*500,f3*500,imagename*500
        integer n,m,l,l0

        if (runcode(2:2).eq.'q') write (*,*) '  Calculating rms image'
        if (l.gt.1) write (*,*) '  Using 2d array for 3d image !!!'
        call rmsd(f1,f2,n,m,runcode,scratch,imagename)

c! next step
        if (runcode(1:1).eq.'m') then
         write (*,*) 
         write (*,'(a,$)') '   Continue (form islands : form) (y)/n ? '
         read (*,'(a1)') ch1
         if (ichar(ch1).eq.32) ch1='y'
333      continue
         f3=""
         if (ch1.eq.'y') call formislands(f1,f3,runcode,scratch)
        end if

        return
        end
c!
c! -------------------------------- SUBROUTINES ----------------------------
c!
        subroutine rmsd(f1,f2,n,m,runcode,scratch,imagename)
        implicit none
<<<<<<< HEAD
        include "wcs_bdsm.inc"
=======
        include "includes/wcs_bdsm.inc"
>>>>>>> Updating source file list for noise; cleanining up include statements, as we no longer allow for symbolic links inside the source directory.
        character f1*500,extn*20,keyword*500,keystrng*500,f2*500
        character comment*500,dir*500,fg*500,scratch*500,runcode*2
        character imagename*500,fn*500,ctype(3)*8,rmsmap*500
        integer n,m,boxsize,stepsize,error
        real*8 nsig,dumr,keyvalue,blankn,blankv,outsideuniv_num
        real*8 crpix(3),cdelt(3),crval(3),crota(3),bm_pix(3)
        integer wcslen,nchar
        parameter (wcslen=450)
        integer wcs(wcslen)
        logical exists

c! read image and headers
        extn='.bparms'
        keyword='boxsize_th'
        call get_keyword(f2,extn,keyword,keystrng,dumr,
     /       comment,'r',scratch,error)
        boxsize=dumr
        extn='.bparms'
        keyword='stepsize_th'
        call get_keyword(f2,extn,keyword,keystrng,dumr,
     /       comment,'r',scratch,error)
        stepsize=dumr

        fg="paradefine"
        extn=""
        dir="./"
        keyword="rms_map"
        call get_keyword(fg,extn,keyword,rmsmap,keyvalue,
     /    comment,"s",dir,error)
        if (rmsmap.ne.'const'.and.rmsmap.ne.'map'
     /      .and.rmsmap.ne.'existing') rmsmap='default'

        if (rmsmap.ne.'existing') then
c! in case there are blanked pixels and outside universe pixels
         extn='.bstat'
         keyword='blank_num'
         call get_keyword(f2,extn,keyword,keystrng,blankn,
     /        comment,'r',scratch,error)
         extn='.bstat'
         keyword='blank_val'
         call get_keyword(f2,extn,keyword,keystrng,blankv,
     /        comment,'r',scratch,error)

c! get wcs
         fn=scratch(1:nchar(scratch))//f2(1:nchar(f2))//'.header'
         inquire(file=fn,exist=exists)
         call read_dum_head(3,ctype,crpix,cdelt,crval,crota,bm_pix)
         fn=f2(1:nchar(f2))//'.header'
         if (exists) call read_head_coord(fn(1:nchar(fn)),3,
     /      ctype,crpix,cdelt,crval,crota,bm_pix,scratch)
         keyword='outsideuniv_num'
         call get_keyword(f2,extn,keyword,keystrng,outsideuniv_num,
     /        comment,'r',scratch,error)
         call wcs_struct(3,ctype,crpix,cdelt,crval,crota,wcs,wcslen)
 
c!
c! create mean and rms images
c! ok. changed for insideuniverse and cant change all of nonmask again
c! so this 
         if (blankn.eq.0.d0) then
          call bdsm_boxnoise(imagename,f2,boxsize,stepsize,n,m,runcode,
     /         outsideuniv_num,wcslen,wcs)
c          call bdsm_boxnoisemask(imagename,f2,boxsize,stepsize,n,m,
c     /         runcode,blankv,outsideuniv_num,wcslen,wcs)
         else
          call bdsm_boxnoisemask(imagename,f2,boxsize,stepsize,n,m,
     /         runcode,blankv,outsideuniv_num,wcslen,wcs)
         end if
c         call freewcs(wcs,wcslen)
        end if ! if not use existing image
c!
c! check if variation is 'real'
        call takemaps(f1,f2,n,m,boxsize,scratch,runcode,imagename,
     /       rmsmap,blankn,blankv)
        

        return
        end
c!
c!      ----------
c!
        subroutine takemaps(f1,f2,n,m,boxsize,scratch,runcode,imagename,
     /             rmsmap,blankn,blankv)
        implicit none
        integer nchar,n,m,boxsize,error,i,j
        character f1*500,extn*20,f2*500,keyword*500,rmsmap*500
        character runcode*2,f3*500,imagename*500,f4*500
        character keystrng*500,comment*500,mean_map*500
        character dir*500,scratch*500,fg*500,confused*500
        real*8 rmsimage(n,m),stdsub,avsub,fw_pix
        real*8 avimage(n,m),stdsubav,avsubav
        real*8 std_im,av_im,bmaj,bmin,cdelt(3),keyvalue,image(n,m)
        real*8 bmpersrc_th,dumr,blankn,blankv
        real*8 pcrit,sigcrit


c! if rms_map is const, use rms_clip as estimator of constant rms 
c! else is map then use f1.rmsd for rms map, after filtering if u want (?)
        extn='.header'
        keyword='CDELT1'
        call get_keyword(f2,extn,keyword,keystrng,cdelt(1),
     /       comment,'r',scratch,error)
        keyword='CDELT2'
        call get_keyword(f2,extn,keyword,keystrng,cdelt(2),
     /       comment,'r',scratch,error)
        keyword='BMAJ'
        call get_keyword(f2,extn,keyword,keystrng,bmaj,
     /       comment,'r',scratch,error)
        keyword='BMIN'
        call get_keyword(f2,extn,keyword,keystrng,bmin,
     /       comment,'r',scratch,error)
        fw_pix=sqrt(abs(bmaj*bmin/cdelt(1)/cdelt(2)))

        extn='.bstat'
        keyword='rms_clip'
        call get_keyword(f2,extn,keyword,keystrng,std_im,
     /       comment,'r',scratch,error)
        if (rmsmap.eq.'default'.or.rmsmap.eq.'existing') then
         extn='.img'
         f3=f2(1:nchar(f2))//'.rmsd'
         call readarray_bin(n,m,rmsimage,n,m,f3,extn)
         call arrstat(rmsimage,n,m,boxsize/2,boxsize/2,n-boxsize/2,
     /        m-boxsize/2,stdsub,avsub)

         if (stdsub.gt.1.1d0*std_im/1.4142d0/boxsize*fw_pix) then
          rmsmap='map  '
         else
          rmsmap='const'
         end if
        end if ! if rmsmap=default in paradefine

c! do the same for mean as well and see. important in cases with actual
c! variation in mean image (zero spacing flux etc)
        extn=''
        f4='paradefine'
        dir = './'
        keyword="mean_map"
        call get_keyword(f4,extn,keyword,mean_map,keyvalue,
     /    comment,"s",dir,error)
        extn='.bstat'
        keyword='mean_clip'
        call get_keyword(f2,extn,keyword,keystrng,av_im,
     /       comment,'r',scratch,error)
        extn='.bparms'
        keyword='confused'
        call get_keyword(f2,extn,keyword,keystrng,confused,
     /       comment,'s',scratch,error)
        if (mean_map.eq.'default') then
         if (confused.eq.'true') then
           mean_map='zero'
         else
          extn='.img'
          f3=f2(1:nchar(f2))//'.mean'
          call readarray_bin(n,m,avimage,n,m,f3,extn)
          call arrstat(avimage,n,m,boxsize/2,boxsize/2,n-boxsize/2,
     /       m-boxsize/2,stdsubav,avsubav)
          if (stdsub.gt.1.1d0*std_im/boxsize*fw_pix) then
           rmsmap='map  '
          else
           rmsmap='const'
          end if
         end if
        end if

        extn='.bparms'
        keyword='rms_map'
        comment=" 'Use constant rms value or rms map'"
        call put_keyword(f2,extn,keyword,rmsmap,0.d0,
     /       comment,'S',scratch)
        keyword='mean_map'
        comment=" 'Use constant mean value or mean map'"
        call put_keyword(f2,extn,keyword,mean_map,0.d0,
     /       comment,'S',scratch)

        return
        end



