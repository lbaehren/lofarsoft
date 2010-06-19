c! take .pas file and plot each against the other

        subroutine plot_associatelist1(scrat,scratch)
        implicit none
        character scrat*500,scratch*500
        integer nlines
        
        call getnlines_pasl(scrat,scratch,nlines)
        if (mod(nlines,2).ne.0) then
         write (*,*) ' !!!!!  ERROR !!!'
        else
         call sub_plot_associatelist1(scrat,nlines/2,scratch)
        end if
        
        return
        end
c!
c! -----------------------------  SUBROUTINES  -------------------------------
c!
        subroutine sub_plot_associatelist1(scrat,npairs,scratch)
        implicit none
<<<<<<< HEAD
        include "constants.inc"
=======
        include "includes/constants.inc"
>>>>>>> Updating source file list for noise; cleanining up include statements, as we no longer allow for symbolic links inside the source directory.
        character scrat*500,fn*500,extn*20,label(100)*30,scratch*500
        character chr1*1,xl*6,yl*6,str*500,rcode*2,headfile*500,cp*1
        character clip*1
        integer nchar,npairs,i,idm(npairs),ids(npairs),id(2),iquit
        integer hh,mm,dd,ma
        real*8 var(100,npairs),dumr,ss,sa,ra,dec,refx,refy,x,y
        real*4 xplot(npairs),yplot(npairs)
        
        data label/'Peak (srl 1) ','R.A. (srl 1)','Dec (srl 1)',
     /    'Bmaj (srl 1)','Bmin (srl 1)','Bpa (srl 1)','Peak (srl 2)',
     /    'R.A. (srl 2)','Dec (srl 2)','Bmaj (srl 2)','Bmin (srl 2)',  ! 11
     /    'Bpa (srl 2)','Distance ','Peak ratio (srl 1/2)',
     /    'Bpa diff (srl 1-2)','Bmaj ratio (srl 1/2)',
     /    'Bmin ratio (srl 1/2)','Src id (srl 1)','Src id (srl 2)',    ! 19
     /    'src residrms','isl residrms','Total (srl 1)','eTot (srl 1)',
     /    'ePeak (srl 1)','Total (srl 2)','eTot (srl 2)',
     /    'ePeak (srl 2)','Ratio total (srl 1/2)','RAdiff (srl 1-2)',  !  29
     /    'DECdiff (srl 1-2)','Peak diff','experimental',
     /    'Distance from a posn','id num','Spectral index (peak)',    ! 36
     /    'Spectral index (total)','Total/Peak (srl 1)',
     /    'Total/Peak (srl 2)','Bmaj*BminM','Bmaj*BminS',
     /    'Bmaj (srl 1)/beam_maj','Bmin (srl 1)/beam_min',
     /    'Bmaj (srl 2)/beam_maj','Bmin (srl 2)/beam_min',
     /    'RMS (srl 1)','RMS (srl 2)','SNR (srl 1)','SNR (srl 2)',
     /    'TotalM/PeakS','TotalS/PeakM','R_diff (pix)',
     /    'Theta_diff (deg)','Radial dist','Azimuth angle',
     /    'posn diff (distance)','Posn diff (angle)','','','','',
     /    '','','','','','','','','','',
     /    '','','','','','','','','','',
     /    '','','','','','','','','','',
     /    '','','','','','','','','',''/

        str=' '
        rcode='v '  ! v is verbose, asks user; q is saved in device str
        extn='.pasf'
        fn=scratch(1:nchar(scratch))//scrat(1:nchar(scrat))//
     /     extn(1:nchar(extn))
        call readin_pasl(fn,scrat,scratch,npairs,var,100)

        call writemenu_plotasrl(iquit)

        write (*,*) 
333     write (*,'(a,$)') '  Enter numbers to plot, (d)efault/(u)ser : '
        read (*,*) id(1),id(2),chr1
        if (max(id(1),id(2)).gt.iquit) goto 333
        if (id(1).ne.iquit.and.id(2).ne.iquit) then
         if (id(1).eq.33.or.id(2).eq.33) then    ! dist from ref posn
          write (*,*) '  Enter reference position : '
          read (*,*) hh,mm,ss,dd,ma,sa
          ra=(hh+mm/60.d0+ss/3600.d0)*15.d0
          call convertdecinv(dd,ma,sa,dec)
          do i=1,npairs
           call justdist(ra,var(8,i),dec,var(9,i),var(33,i))
          end do
         end if

c        posn diff in polar coords around a centre. cant do it ra, dec 
c        since thats rubbish for very big images, esp near the pole. 
c        hence will do for a particular wcs in pixel space.
         if (id(1).eq.51.or.id(2).eq.51.or.
     /       id(1).eq.52.or.id(2).eq.52) then    ! posn diff in r, theta
          write (*,'(a,$)') 
     /       '  Filename (.header) for wcs (calc in pixel space) : '
          read (*,*) headfile
          write (*,'(a,$)') '  Enter reference position : '
          read (*,*) hh,mm,ss,dd,ma,sa
          ra=(hh+mm/60.d0+ss/3600.d0)*15.d0
          call convertdecinv(dd,ma,sa,dec)
          call sub_pasl_polar(var,100,npairs,headfile,ra,dec,scratch)
         end if

c! more useful -- take posn diff vector and calc mag and angle in pixel space
         if (id(1).eq.55.or.id(2).eq.55.or.
     /       id(1).eq.56.or.id(2).eq.56) then    ! posn diff in mag, ang
          write (*,'(a,$)') 
     /       '  Filename (.header) for wcs (calc in pixel space) : '
          read (*,*) headfile
          call sub_pasl_posndiff_magang(var,100,npairs,headfile,scratch)
         end if


c! radial dist and azimuth around a point. in pixel space
         if (id(1).eq.53.or.id(2).eq.53.or.
     /       id(1).eq.54.or.id(2).eq.54) then    ! posn diff in r, theta
667       write (*,'(a,$)') 
     /       '  Reference position in (c)oord or (p)ixel : '
          read (*,*) cp 
          if (cp.ne.'c'.and.cp.ne.'p') goto 667
          write (*,'(a,$)') 
     /       '  Filename (.header) for wcs (calc in pixel space) : '
          read (*,*) headfile
          if (cp.eq.'p') then
           write (*,'(a,$)') '  Reference pixel : '
           read (*,*) refx, refy
          else
           write (*,'(a,$)') '  Enter reference position : '
           read (*,*) hh,mm,ss,dd,ma,sa
           ra=(hh+mm/60.d0+ss/3600.d0)*15.d0
           call convertdecinv(dd,ma,sa,dec)
          end if
          call sub_pasl_radazi(var,100,npairs,headfile,scratch,
     /         refx,refy)
         end if

         call setup_pasl(xl,yl,npairs,var,100,xplot,yplot,id)

         clip='n'
         call plot_menu(xplot,yplot,npairs,label(id(1)),
     /        label(id(2)),chr1,xl,yl,str,rcode,scrat,clip)
         goto 333
        end if
        
        return
        end
c!
c!      ----------
c!
c!
        subroutine plot_menu(xplot,yplot,n,label1,label2,chr1,xl,yl,
     /             fname,rcode,scrat,clip)
        implicit none
        integer n,nc,i,nchar,mm,pp,pgopen,id,mask(n),dumi
        real*4 xplot(n),yplot(n),mnx,mxx,mny,mxy,mny1,mxy1
        real*4 minxo,maxxo,mnx1,mxx1,dum41,dum42,minyo,maxyo
        real*4 uminx,umaxx,uminy,umaxy,avx,stdx,avy,stdy,medx,medy
        character label1*30,label2*30,chr1*1,chrid*40,xl*6,yl*6
        character rcode*2,fname*500,dev*500,scrat*500,str1*500,clip*1
        logical mark
<<<<<<< HEAD

cf2py   intent(in) xplot, yplot, label1, label2, chr1, xl, yl
cf2py   intent(in) fname, rcode, scrat

=======

cf2py   intent(in) xplot, yplot, label1, label2, chr1, xl, yl
cf2py   intent(in) fname, rcode, scrat

>>>>>>> Updating source file list for noise; cleanining up include statements, as we no longer allow for symbolic links inside the source directory.
        call sub_pasl_clip(clip,xplot,yplot,n,mask)

        call range_vec4mxmn_mask(xplot,mask,n,n,mnx,mxx)
        call range_vec4mxmn_mask(yplot,mask,n,n,mny,mxy)
        call vec_std4(xplot,n,1,n,avx,stdx)
        call vec_std4(yplot,n,1,n,avy,stdy)
        call calcmedian4(xplot,n,1,n,medx)
        call calcmedian4(yplot,n,1,n,medy)
        minxo=mnx
        maxxo=mxx
        minyo=mny
        maxyo=mxy
        if (label1(1:6).eq.label2(1:6)) then
         mnx=min(mnx,mny)
         mny=mnx
         mxx=max(mxx,mxy)
         mxy=mxx
        end if
        call gethistpara(n,n,xplot,minxo,maxxo,mnx1,mxx1,0)
        call gethistpara(n,n,yplot,minyo,maxyo,mny1,mxy1,0)
        mark=.false.

333     continue
        if (rcode(2:2).eq.'q') then
         dev=fname(1:nchar(fname))
         id=pgopen(fname(1:nchar(fname)))
         !call pgbegin(0,dev,1,1)
        else
         id=pgopen('?')
         !call pgbegin(0,'?',1,1)
         !call pgeras
        end if
        
        call pgsch(1.1)
        call pgslw(3)
        call pgvport(0.66,1.0,0.35,0.9)
        call pgsch(0.6)
        call pgtext(0.1,0.8,scrat(1:nchar(scrat)))
        call pgsch(1.3)
        call get_pgnumb(avx,mm,pp)
        call pgnumb(mm,pp,1,str1,nc)
        call pgtext(0.1,0.7,'Av X='//str1(1:nc))
        call get_pgnumb(avy,mm,pp)
        call pgnumb(mm,pp,1,str1,nc)
        call pgtext(0.1,0.6,'Av Y='//str1(1:nc))
        call get_pgnumb(stdx,mm,pp)
        call pgnumb(mm,pp,1,str1,nc)
        call pgtext(0.1,0.5,'Std X='//str1(1:nc))
        call get_pgnumb(stdy,mm,pp)
        call pgnumb(mm,pp,1,str1,nc)
        call pgtext(0.1,0.4,'Std Y='//str1(1:nc))
        call get_pgnumb(medx,mm,pp)
        call pgnumb(mm,pp,1,str1,nc)
        call pgtext(0.1,0.3,'Med X='//str1(1:nc))
        call get_pgnumb(medy,mm,pp)
        call pgnumb(mm,pp,1,str1,nc)
        call pgtext(0.1,0.2,'Med Y='//str1(1:nc))
        call pgiden

        call pgvport(0.1,0.65,0.4,0.95)
        call pgwindow(mnx,mxx,mny,mxy)
        call pgbox(xl(1:nchar(xl)),0.0,0,yl(1:nchar(yl)),0.0,0)
        call pglabel(label1(1:nchar(label1)),
     /       label2(1:nchar(label2)),' ')

        if (n.lt.1000) then
         dumi=3
        else
         dumi=1
        end if

        if (clip.eq.'n') then
         call pgpoint(n,xplot,yplot,dumi)
        else
         do i=1,n
          if (mask(i).eq.1) call pgpoint(1,xplot(i),yplot(i),dumi)
         end do
        end if

        if (label1(1:6).eq.label2(1:6)) then
         call pgmove(mnx,mny)
         call pgdraw(mxx,mxy)
        end if
        if (mark) then
         do 200 i=1,n
          if (xplot(i).ge.mnx.and.xplot(i).le.mxx.and.
     /        yplot(i).ge.mny.and.yplot(i).le.mxy) then
           call pgnumb(i,0,1,chrid,nc)
           call pgtext(xplot(i),yplot(i),chrid)
          end if
200      continue
        end if

        call pgvport(0.05,0.44,0.05,0.25)
        if (label1(1:6).eq.label2(1:6)) then
         call pgwindow(minxo,maxxo,min(mnx1,mny1),max(mxx1,mxy1))
        else
         call pgwindow(minxo,maxxo,mnx1,mxx1)
        end if
        call pgbox('BCNST',0.0,0,'BCVNST',0.0,0)
        call pghist(n,xplot,minxo,maxxo,100,1)
        dum41=minxo+0.1*(maxxo-minxo)
        dum42=mxx1-0.2*(mxx1-mnx1)
        call pgtext(dum41,dum42,label1(1:nchar(label1)))

        call pgvport(0.56,0.95,0.05,0.25)
        if (label1(1:6).eq.label2(1:6)) then
         call pgwindow(minxo,maxxo,min(mnx1,mny1),max(mxx1,mxy1))
        else
         call pgwindow(minyo,maxyo,mny1,mxy1)
        end if
        call pgbox('BCNST',0.0,0,'BCVNST',0.0,0)
        call pghist(n,yplot,minyo,maxyo,100,1)
        dum41=minyo+0.1*(maxyo-minyo)
        dum42=mxy1-0.2*(mxy1-mny1)
        call pgtext(dum41,dum42,label2(1:nchar(label2)))
        call pgclos

        if (chr1.eq.'u') then
         write (*,*) ' Enter Min, max for x and then y '
         write (*,'(a,$)') ' (all 0=default; all 9=src id) : '
         read (*,*) uminx,umaxx,uminy,umaxy
         mark=.false.
         if (uminx.eq.9.and.umaxx.eq.9.and.
     /       uminy.eq.9.and.umaxy.eq.9) then
          mark=.true.
          goto 333
         else
          if (uminx.ne.0.0.or.umaxx.ne.0.0.or.
     /        uminy.ne.0.0.or.umaxy.ne.0.0) then
           if (xl(6:6).eq.'L'.and.uminx.ne.0.0) uminx=log10(uminx)
           if (xl(6:6).eq.'L'.and.umaxx.ne.0.0) umaxx=log10(umaxx)
           if (yl(6:6).eq.'L'.and.uminy.ne.0.0) uminy=log10(uminy)
           if (yl(6:6).eq.'L'.and.umaxy.ne.0.0) umaxy=log10(umaxy)
           if (uminx.eq.0.0) uminx=mnx
           if (umaxx.eq.0.0) umaxx=mxx
           if (uminy.eq.0.0) uminy=mny
           if (umaxy.eq.0.0) umaxy=mxy
           mnx=uminx
           mxx=umaxx
           mny=uminy
           mxy=umaxy
           goto 333
          end if
         end if
        end if

        return
        end
c!
c!
c! take header file for wcs, convert to pixels and then do dR, dtheta
        subroutine sub_pasl_polar(var,n,npairs,headfile,cra,cdec,
     /             scratch)
        implicit none
<<<<<<< HEAD
        include "wcs_bdsm.inc"
        include "constants.inc"
=======
        include "includes/wcs_bdsm.inc"
        include "includes/constants.inc"
>>>>>>> Updating source file list for noise; cleanining up include statements, as we no longer allow for symbolic links inside the source directory.
        integer n,npairs,wcslen,i
        real*8 var(n,npairs),cra,cdec,x1,y1,x2,y2,xcen,ycen
        real*8 r1,r2,th1,th2,ra1,ra2,dec1,dec2
        character headfile*500
        real*8 crpix(3),cdelt(3),crval(3),crota(3)
        real*8 bmaj,bmin,bpa,keyvalue
        character ctype(3)*8,extn*20,keyword*500,keystrng*500
        character comment*500,scratch*500
        parameter (wcslen=450)
        integer wcs(wcslen),error1

cf2py   intent(in) var, n, npairs, headfile, cra, cdec, scratch
cf2py   intent(out) var

        extn=".header"
        call readheader4fits(headfile,extn,ctype,crpix,cdelt,crval,
     /         crota,bmaj,bmin,bpa,3,scratch)
        call wcs_struct(3,ctype,crpix,cdelt,crval,crota,wcs,wcslen)
        call wcs_radec2xy(cra/rad,cdec/rad,xcen,ycen,error1,wcs,wcslen)
        do i=1,npairs
         ra1=var(2,i)/rad
         ra2=var(8,i)/rad
         dec1=var(3,i)/rad
         dec2=var(9,i)/rad
         call wcs_radec2xy(ra1,dec1,x1,y1,error1,wcs,wcslen)
         call wcs_radec2xy(ra2,dec2,x2,y2,error1,wcs,wcslen)
         call cart2polar(x1,y1,xcen,ycen,r1,th1)
         call cart2polar(x2,y2,xcen,ycen,r2,th2)
         var(51,i)=r1-r2
         var(52,i)=(th1-th2)*180.d0/pi
        end do
 
        return
        end

c! calc radial distance and azimuth around refx, refy
        subroutine sub_pasl_radazi(var,n,npairs,headfile,
     /       scratch,refx,refy)
        implicit none
<<<<<<< HEAD
        include "wcs_bdsm.inc"
        include "constants.inc"
=======
        include "includes/wcs_bdsm.inc"
        include "includes/constants.inc"
>>>>>>> Updating source file list for noise; cleanining up include statements, as we no longer allow for symbolic links inside the source directory.
        integer n,npairs,wcslen,i
        real*8 var(n,npairs),refx,refy,ra1,dec1,x1,y1,r1,th1
        character headfile*500
        real*8 crpix(3),cdelt(3),crval(3),crota(3)
        real*8 bmaj,bmin,bpa,keyvalue
        character ctype(3)*8,extn*20,keyword*500,keystrng*500
        character comment*500,scratch*500
        parameter (wcslen=450)
        integer wcs(wcslen),error1

cf2py   intent(in) var, n, npairs, headfile, scratch, refx, refy
cf2py   intent(out) var


        extn=".header"
        call readheader4fits(headfile,extn,ctype,crpix,cdelt,crval,
     /         crota,bmaj,bmin,bpa,3,scratch)
        call wcs_struct(3,ctype,crpix,cdelt,crval,crota,wcs,wcslen)

        do i=1,npairs
         ra1=var(2,i)/rad
         dec1=var(3,i)/rad
         call wcs_radec2xy(ra1,dec1,x1,y1,error1,wcs,wcslen)
         call cart2polar(x1,y1,refx,refx,r1,th1)
         var(53,i)=r1
         var(54,i)=th1*180.d0/pi
        end do

        return
        end

c! calc posn diff as mag and ang in pixel space
        subroutine sub_pasl_posndiff_magang(var,n,npairs,headfile,
     /             scratch)
        implicit none
<<<<<<< HEAD
        include "wcs_bdsm.inc"
        include "constants.inc"
=======
        include "includes/wcs_bdsm.inc"
        include "includes/constants.inc"
>>>>>>> Updating source file list for noise; cleanining up include statements, as we no longer allow for symbolic links inside the source directory.
        integer n,npairs,wcslen,i
        real*8 var(n,npairs),refx,refy,ra1,dec1,x1,y1,r,th
        character headfile*500
        real*8 crpix(3),cdelt(3),crval(3),crota(3)
        real*8 bmaj,bmin,bpa,keyvalue,ra2,dec2,x2,y2
        character ctype(3)*8,extn*20,keyword*500,keystrng*500
        character comment*500,scratch*500
        parameter (wcslen=450)
        integer wcs(wcslen),error1

cf2py   intent(in) var, n, npairs, headfile, scratch
cf2py   intent(out) var

        extn=".header"
        call readheader4fits(headfile,extn,ctype,crpix,cdelt,crval,
     /         crota,bmaj,bmin,bpa,3,scratch)
        call wcs_struct(3,ctype,crpix,cdelt,crval,crota,wcs,wcslen)

        do i=1,npairs
         ra1=var(2,i)/rad
         ra2=var(8,i)/rad
         dec1=var(3,i)/rad
         dec2=var(9,i)/rad
         call wcs_radec2xy(ra1,dec1,x1,y1,error1,wcs,wcslen)
         call wcs_radec2xy(ra2,dec2,x2,y2,error1,wcs,wcslen)
         call cart2polar(x1,y1,x2,y2,r,th)
         var(55,i)=r
         var(56,i)=th*180.d0/pi
        end do
        
        return
        end
c!
c!
c!
        subroutine sub_pasl_clip(clip,xplot,yplot,n,mask)
        implicit none
        integer n,mask(n),maxindx,maxindy
        real*4 xplot(n),yplot(n)
        real*8 x(n),y(n),avx,avx1,stdx,stdx1,medx,medx1
        real*8 avy,avy1,stdy,stdy1,medy,medy1
        character clip

        call initialise_int_vec(mask,n,1)

        call vec4to8(xplot,n,x,n)
        call vec4to8(yplot,n,y,n)
        if (clip.eq.'y') then
         call vec_std(x,n,1,n,avx,stdx)
         call calcmedian(x,n,1,n,medx)
         call argmax(x,n,maxindx)
         x(maxindx)=medx
         call vec_std(x,n,1,n,avx1,stdx1)
         call calcmedian(x,n,1,n,medx1)
         if (stdx1.lt.0.5d0*stdx) mask(maxindx)=0
         
         call vec_std(y,n,1,n,avy,stdy)
         call calcmedian(y,n,1,n,medy)
         call argmax(y,n,maxindy)
         y(maxindy)=medy
         call vec_std(y,n,1,n,avy1,stdy1)
         call calcmedian(y,n,1,n,medy1)
         if (stdy1.lt.0.5d0*stdy) then
          mask(maxindy)=0
         end if
        end if

        return
        end










