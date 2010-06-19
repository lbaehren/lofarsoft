c! this creates sources, convolves with different psfs and puts noise, and makes the
c! source list. To test BDSM with, exactly. copy most stuff from simulation.f

        subroutine cr8sources(filename,seed,scratch,srldir,fitsdir)
        implicit none
<<<<<<< HEAD
        include "constants.inc"
=======
        include "includes/constants.inc"
>>>>>>> Updating source file list for noise; cleanining up include statements, as we no longer allow for symbolic links inside the source directory.
        character filename*500,scratch*500,srldir*500,fitsdir*500
        real*8 fwhm,beampersrc,snrmin,snrmax,bmareamin,bmareamax
        real*8 dumr1,dumr2,ratmin,ratmax,dumr,bmarea
        integer n,m,seed,maxsize,round,nchar,nsrc

cf2py   intent(in) filename,scratch,srldir,fitsdir
cf2py   intent(in,out) seed

        write (*,*) '  This will create an image and a sourcelist.'
        write (*,*) 
        call sub_cr8sources_gethead(n,m,fwhm,beampersrc,snrmin,
     /             snrmax,bmareamin,bmareamax,ratmin,ratmax)

        dumr=fwhm*sqrt(ratmax*bmareamax)
        maxsize=2*round(dumr/fwsig*sqrt(2.d0*dlog(snrmax/0.3d0)))+1  ! down to 0.3 sigma
        maxsize=maxsize*2

c! calculate parameters for the image
        bmarea=2.d0*pi/fwsig/fwsig*fwhm*fwhm  ! in pix^2
        nsrc=round(n*m/(beampersrc*bmarea))
        if (nsrc.lt.1) nsrc=1
        write (*,*) '  Creating ',nsrc,' sources'

        call sub_cr8sources(filename,n,m,fwhm,beampersrc,snrmin,
     /    snrmax,bmareamin,bmareamax,ratmin,ratmax,seed,maxsize,
     /    scratch,srldir,fitsdir,nsrc,bmarea)
        
        return
        end

c!      --------------------------------   SUBROUTINES   -------------------------------

        subroutine sub_cr8sources(filename,n,m,fwhm,beampersrc,snrmin,
     /     snrmax,bmareamin,bmareamax,ratmin,ratmax,seed,msize,
     /     scratch,srldir,fitsdir,nsrc,bmarea)
        implicit none
<<<<<<< HEAD
        include "constants.inc"
        include "wcs_bdsm.inc"
=======
        include "includes/constants.inc"
        include "includes/wcs_bdsm.inc"
>>>>>>> Updating source file list for noise; cleanining up include statements, as we no longer allow for symbolic links inside the source directory.
        integer n,m,nsrc,round,seed,msize,xc,yc,xca,yca
        integer nchar,isrc,i,j,dumi,wid,flag(1),sqrtn
        real*8 fwhm,beampersrc,snrmin,snrmax,bmareamin,bmareamax
        real*8 image(n,m),bmarea,ratmin,ratmax,subim(msize,msize)
        real*8 rand,area,ratio,a(6),cnoise(n,m),fac,s1,bm_pix(3)
        character filename*500,fn*500,prog*20,ffmt*500,scratch*500
        real*8 crpix(3),cdelt(3),crval(3),crota(3),thresh_pix
        real*8 noise(n,m),sig_c(3),std,av,deconv_s(3,1)
        character ctype(3)*8,bcon*10,bcode*4,rmsmap*500,keyword*500
        character extn*20,comment*500,flag_s(1),srldir*500,fn1*500
        character fn2*500,fn3*500,fn4*500,scode(nsrc)*4
        character fitsdir*500,fitsname*500,dumc*500,wcsimp*1
        real*8 e_amp(1),e_x0(1),e_y0(1),e_maj(1),e_min(1)
        real*8 e_pa(1),e_tot(1),dsize,peakfl(nsrc),bpa(nsrc)
        real*8 xpos(nsrc),ypos(nsrc),dist,bmj(nsrc),bmn(nsrc)
        real*8 ipeak(2),ixpos(2),iypos(2),ibmj(2),ibmn(2),ibpa(2)
        character calctot*1
        logical same_island
        integer wcslen,blc(2),trc(2)
        parameter (wcslen=450)
        integer wcs(wcslen)
        real*8 dumr1,dumr2,dumr3,dumr4,dumr5,dumr6

        rmsmap='const'
        thresh_pix=snrmin
c! initialise image, open sourcelist file (.ini.gaul == real srl), fix headers
        call initialiseimage(image,n,m,n,m,0.d0)
        dumc=srldir(1:nchar(srldir))//filename(1:nchar(filename))
        fn=dumc(1:nchar(dumc))//'.ini.gaul'
        fn1=dumc(1:nchar(dumc))//'.ini.gaul.bin'
        open(unit=23,file=fn(1:nchar(fn)),status='unknown')
        open(unit=27,file=fn1(1:nchar(fn1)),form='unformatted') 
        prog='cr8sources.f'
        call sub_sourcemeasure_writehead(23,filename,filename,n,m,nsrc,
     /       1.d0,prog,rmsmap,thresh_pix)
        call read_dum_head(3,ctype,crpix,cdelt,crval,crota,bm_pix)
        crpix(1)=int(n/2)
        crpix(2)=int(m/2)
        call writefitshead(filename,3,crpix,ctype(1),ctype(2),ctype(3),
     /       cdelt,crval,crota,bm_pix,scratch) ! write to header file
c! prepare WCS struct for wcslib to pass on.
        call wcs_struct(3,ctype,crpix,cdelt,crval,crota,wcs,wcslen)

c! for each source ... 
        dumi=0  ! hack for put_srclist
        sqrtn=int(sqrt(1.d0*nsrc))
        do 100 isrc=1,nsrc

c! get random source parameters 
         call sub_cr8sources_rand_a(seed,a,snrmin,snrmax,n,m,bmareamin,
     /        bmareamax,ratmin,ratmax,bmarea)
         
c        call ran1(seed,rand)
c        a(2)=((isrc-1)/sqrtn+1)*(n/sqrtn)+(rand*2.d0-1.d0)-0.3*n/sqrtn
c        call ran1(seed,rand)
c        a(3)=(isrc-((isrc-1)/sqrtn)*sqrtn)*(m/sqrtn)+(rand*2.d0-1.d0)
c     /        -0.3*n/sqrtn

c! get the subimage of delta fn convolved with elliptical gaussian
         call get_subim_xsrc(subim,msize,msize,a(4)*fwsig,a(5)*fwsig,
     /       a(1),a(6),a(2),a(3),xc,yc)
       
c! add subimage to image (copied from cr8sky.f)
         xca=round(a(2))-xc
         yca=round(a(3))-yc
         do j=1,msize
          do i=1,msize
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
         blc(1)=round(a(2))-bm_pix(1)*2
         blc(2)=round(a(3))-bm_pix(1)*2
         trc(1)=round(a(2))+bm_pix(1)*2
         trc(2)=round(a(3))+bm_pix(1)*2
         dumr1=0.d0
         dumr2=0.d0
         dumr3=0.d0
         dumr4=0.d0
         dumr5=0.d0
         dumr6=0.d0
         wcsimp='y'
         calctot='y'
         call putin_srclist(23,27,isrc,dumi,a,1,6,1,0.d0,0.d0,0.d0,0.d0,
     /     0.d0,0.d0,0,0,ffmt,filename,ctype,crpix,cdelt,crval,crota,
     /     deconv_s,0,flag,flag_s,e_amp,e_x0,e_y0,e_maj,e_min,e_pa,
     /     e_maj,e_min,e_pa,e_tot,scratch,bm_pix,.false.,wcs,wcslen,
     /     blc,trc,dumr1,dumr2,dumr3,dumr4,dumr5,dumr6,wcsimp,calctot)
         peakfl(isrc)=a(1)   ! to use in same_island
         xpos(isrc)=a(2)   ! to use in same_island
         ypos(isrc)=a(3)
         bmj(isrc)=a(4)*fwsig
         bmn(isrc)=a(5)*fwsig
         bpa(isrc)=a(6)
         scode(isrc)=' S '

100     continue  !  for each source
        close(23)
        close(27)
        call sub_sourcemeasure_writeheadmore(nsrc,1,fn,ffmt,scratch)

c! now add srcctr to the last column of .gaul 
c! CHANGED putin_srclist so dont need this anymore
c        fn4=filename(1:nchar(filename))//'.ini'
c        call cr8src_addsrcid(srldir,scratch,fn4,nsrc)

c! write srl also. Use same_island routine in callgaul2srl.f to identify code since they
c! could overlap. 
        do i=1,nsrc-1
         do j=i+1,nsrc
          dist=sqrt((xpos(i)-xpos(j))*(xpos(i)-xpos(j))+
     /         (ypos(i)-ypos(j))*(ypos(i)-ypos(j)))
          dsize=bmj(i)+bmj(j)
          if (dist.lt.4.d0*dsize) then  ! then check 
           ipeak(1)=peakfl(i)
           ipeak(2)=peakfl(j)
           ixpos(1)=xpos(i)
           ixpos(2)=xpos(j)
           iypos(1)=ypos(i)
           iypos(2)=ypos(j)
           ibmj(1)=bmj(i)
           ibmj(2)=bmj(j)
           ibmn(1)=bmn(i)
           ibmn(2)=bmn(j)
           ibpa(1)=bpa(i)
           ibpa(2)=bpa(j)
           if (same_island(1,2,2,2,ipeak,ixpos,iypos,ibmj,  ! 3sigma, rms=1 Jy
     /         ibmn,ibpa,3.d0,1.d0,cdelt)) then  
            scode(i)=' M '
            scode(j)=' M '
           end if
          end if
         end do
        end do

        fn2=dumc(1:nchar(dumc))//'.ini.srl'
        fn3=dumc(1:nchar(dumc))//'.ini.srl.bin'
        open(unit=28,file=fn2(1:nchar(fn2)),form='formatted') 
        open(unit=29,file=fn3(1:nchar(fn3)),form='unformatted') 
        call putsrlhead(28,nsrc,filename,1,n,m)
        do i=1,nsrc
         write (28,*) i,i,i,scode(i)
         write (29) i,i,i,scode(i)
        end do
        close(28)
        close(29)

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
        call writearray_bin2D(cnoise,n,m,n,m,fn,'mv')

c! add noise map to source map and write out
        do j=1,m
         do i=1,n
          image(i,j)=image(i,j)+cnoise(i,j)
         end do
        end do
        call writearray_bin2D(image,n,m,n,m,filename,'mv')
        write (*,*) '  Writing sourcelist ',filename(1:nchar(filename))
     /              //'.ini.gaul'
        fitsname='header'

        call writefits(image,n,m,n,m,filename,fitsdir,fitsdir,
     /             scratch,fitsname)
        call freewcs(wcs,wcslen)

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
        include "includes/constants.inc"
        integer n,m,seed
        real*8 fwhm,beampersrc,snrmin,snrmax,bmareamin,bmareamax
        real*8 a(6),rand,ratio,area,ratmin,ratmax,bmarea,low,hi

        call ran1(seed,rand)
        low=5.74d5*(snrmin**(-1.184d0))
        hi=5.74d5*(snrmax**(-1.184d0))
        a(1)=((low-(low-hi)*rand)/5.74d5)**(1.d0/(-1.184)) 
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

        subroutine cr8src_addsrcid(srldir,scratch,f2,nsrc)
        implicit none
        integer nsrc,nchar,headint,i
        character f2*500,scratch*500,srldir*500,fname*500,fnameb*500
        character ftemp*500,ftempb*500,head*1000,ffmt*500
        integer snum,gauln,isl,flag,dumi,blc1,blc2,trc1,trc2
        real*8 totfl,etotfl,peakfl,epeakfl,ra,era,dec,edec,xpix,expix
        real*8 ypix,eypix,bmaj,ebmaj,bmin,ebmin,bpa,ebpa,dbmaj,edbmaj
        real*8 dbmin,edbmin,dbpa,edbpa,rmssrc,avsrc,rmsisl,avisl,chisq,q
        real*8 dumr1,dumr2,dumr3,dumr4,dumr5,dumr6

        fname=srldir(1:nchar(srldir))//f2(1:nchar(f2))//'.gaul'
        fnameb=srldir(1:nchar(srldir))//f2(1:nchar(f2))//'.gaul.bin'
        ftemp=srldir(1:nchar(srldir))//f2(1:nchar(f2))//'.gaul.scr'
        ftempb=srldir(1:nchar(srldir))//f2(1:nchar(f2))//'.gaul.scrb'
        open(unit=22,file=fname)
        open(unit=23,file=fnameb,form='unformatted')
        open(unit=24,file=ftemp)
        open(unit=27,file=ftempb,form='unformatted')

335     read (22,'(a)') head
        write (24,'(a)') head(1:nchar(head))
        if (head(2:4).ne.'fmt') goto 335
        backspace(22)
        read (22,*) head,headint,ffmt
        do i=1,nsrc
         read (23) gauln,isl,flag
         snum=i
         backspace(23)
         read (23) gauln,isl,flag,totfl,etotfl,peakfl,epeakfl,ra,era,
     /    dec,edec,xpix,expix,ypix,eypix,bmaj,ebmaj,bmin,ebmin,bpa,
     /    ebpa,dbmaj,edbmaj,dbmin,edbmin,dbpa,edbpa,rmssrc,avsrc,
     /    rmsisl,avisl,chisq,q,dumi,blc1,blc2,trc1,trc2,
     /    dumr1,dumr2,dumr3,dumr4,dumr5,dumr6
         write (24,ffmt) gauln,isl,flag,totfl,etotfl,peakfl,epeakfl,
     /    ra,era,dec,edec,xpix,expix,ypix,eypix,bmaj,ebmaj,bmin,ebmin,
     /    bpa,ebpa,dbmaj,edbmaj,dbmin,edbmin,dbpa,edbpa,rmssrc,avsrc,
     /    rmsisl,avisl,chisq,q,snum,blc1,blc2,trc1,trc2,
     /    dumr1,dumr2,dumr3,dumr4,dumr5,dumr6
         write (27) gauln,isl,flag,totfl,etotfl,peakfl,epeakfl,ra,era,
     /    dec,edec,xpix,expix,ypix,eypix,bmaj,ebmaj,bmin,ebmin,bpa,ebpa,
     /    dbmaj,edbmaj,dbmin,edbmin,dbpa,edbpa,rmssrc,avsrc,rmsisl,
     /    avisl,chisq,q,snum,blc1,blc2,trc1,trc2,
     /    dumr1,dumr2,dumr3,dumr4,dumr5,dumr6
        end do
        close(22)
        close(23)
        close(24)
        close(27)
        close(28)
        call system('mv -f '//ftemp(1:nchar(ftemp))//' '//
     /       fname(1:nchar(fname)))
        call system('mv -f '//ftempb(1:nchar(ftempb))//' '//
     /       fnameb(1:nchar(fnameb)))
        
        return
        end



