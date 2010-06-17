c! overplot pasf file

        subroutine overplotpasf(scrat,f1,scratch,n,
     /     starc,id1,id2,str1,hi,blc,trc,arr4,ii,jj,up,low,tr,lab,
     /     cod1,mn1,mx1,mn2,mx2,lowh,uph,dum4,colourscheme,blacki)
        implicit none
        character scrat*500,fn*500,extn*20,scratch*500,cmd*500,f1*500
        character str1*5,lab*500,cod1*2,colourscheme*20
        integer nchar,nlines,n,starc,id1,id2
        integer ii,jj,hi,dumi,blacki
        real*4 arr4(ii,jj),tr(6),low,up,mn1,mx1,mn2,mx2
        real*4 blc(2),trc(2),dum4,uph,lowh
        
        extn='.pasf'
        fn=scratch(1:nchar(scratch))//scrat(1:nchar(scrat))//
     /     extn(1:nchar(extn))
        call system('rm -f '//scratch(1:nchar(scratch))//'aa')
        cmd='wc -l '//fn(1:nchar(fn))//' > '//
     /       scratch(1:nchar(scratch))//'aa'
        call system(cmd)
        open(unit=31,file=scratch(1:nchar(scratch))//'aa',status='old')
        read(31,*) nlines
        close(31)
        nlines=nlines-1
        if (mod(nlines,2).ne.0) then
         write (*,*) ' !!!!!  ERROR !!!'
        else
         call sub_overplot_pasl(scrat,nlines/2,scratch,f1,n,
     /        starc,id1,id2,str1,hi,blc,trc,arr4,ii,jj,up,low,tr,lab,
     /        cod1,mn1,mx1,mn2,mx2,lowh,uph,dum4,colourscheme,blacki)
        end if
        call system('rm -f '//scratch(1:nchar(scratch))//'aa')
        
        return
        end
c!
c! -----------------------------  SUBROUTINES  -------------------------------
c!
        subroutine sub_overplot_pasl(scrat,npairs,scratch,f1,n,
     /        starc,id1,id2,str1,hi,blc,trc,arr4,ii,jj,up,low,tr,lab,
     /        cod1,mn1,mx1,mn2,mx2,lowh,uph,dum4,colourscheme,blacki)
        implicit none
        include "constants.inc"
        character scrat*500,fn*500,extn*20,label(40)*30,scratch*500
        character chr1*1,xl*6,yl*6,f2*500,starname*500,str1*5,fi*500
        integer nchar,npairs,i,idm(npairs),ids(npairs),id(2)
        integer n,iquit,error,starc,nc,nstar,id1,id2
        real*8 var(40,npairs),dumr,x,y,x1,y1
        real*4 x41(npairs),x42(npairs),y41(npairs),y42(npairs),sizep,l1
        real*4 xp1,yp1,xp2,yp2,slop,intr
        real*4 xd1,xd2,yd1,yd2,stfac
        character hdrfile*500,ctype(n)*8,f1*500,choi,str2*10,nam*500
        real*8 crpix(n),cdelt(n),crval(n),crota(n),bm_pix(n)
        logical exists,logged
        integer nopts,nmaxopts,id3,pgopen,nisl,nsrc,gpi,nffmt
        PARAMETER (nmaxopts=9)
        real*4 xa(nmaxopts),ya(nmaxopts)
        character ch1*1,opts(nmaxopts)*100,ffmt*500,ch2*1
        integer ii,jj,hi,dumi,dumi1
        real*4 arr4(ii,jj),tr(6),low,up,mn1,mx1,mn2,mx2,dmaj,dmin
        real*4 blc(2),trc(2),dum4,uph,lowh,mnf,mxf,dm41,dm42,dpa
        character lab*500,cod1*2,colourscheme*20,master*30,slave*30
        integer wcslen,blacki
        parameter (wcslen=450)
        integer wcs(wcslen)

        data opts/'Circle associations','Posn shift arrows',
     /            'Print flux ratios','Plot flux ratios',
     /            'Print master name',
     /            'Print slave name','Clear graphics','Quit',''/
        
        dumi=0
        do i=1,nchar(scrat)-1
         if (scrat(i:i+1).eq.'__') dumi=i
        end do
        if (dumi.le.1.or.dumi.gt.nchar(scrat)-2) 
     /      write (*,*) '  WRONG NAME !!!!'
        master=scrat(1:dumi-1)
        slave=scrat(dumi+2:nchar(scrat))

        extn='.pasf'
        fn=scratch(1:nchar(scratch))//scrat(1:nchar(scrat))//
     /     extn(1:nchar(extn))
        open(unit=21,file=fn(1:nchar(fn)),status='old')
        read (21,*) 
        do i=1,npairs
         read (21,*) var(18,i),var(22,i),var(23,i),var(1,i),var(24,i),
     /    var(2,i),var(3,i),var(4,i),
     /    var(5,i),var(6,i),var(20,i),var(21,i)  ! posns are in deg, (2,3) and (8,9)
         read (21,*) var(19,i),var(25,i),var(26,i),var(7,i),var(27,i),
     /    var(8,i),var(9,i),var(10,i),var(11,i),var(12,i),var(13,i)
        end do
        close(21)

        hdrfile=f1(1:nchar(f1))//'.header'
        call read_head_coord(hdrfile,n,ctype,crpix,cdelt,
     /                             crval,crota,bm_pix,scratch)
        call wcs_struct(3,ctype,crpix,cdelt,crval,crota,wcs,wcslen)

        do i=1,npairs
         call wcs_radec2xy(var(2,i)/rad,var(3,i)/rad,x,y,error,
     /        wcs,wcslen)
         call wcs_radec2xy(var(8,i)/rad,var(9,i)/rad,x1,y1,error,
     /        wcs,wcslen)
         x41(i)=x
         y41(i)=y 
         x42(i)=x1
         y42(i)=y1
        end do 

        id3=pgopen(str1(1:nchar(str1)))  ! menu
        if (id3.le.0) stop
        call pgslct(id3)
        call pgpap(4.0,1.0)
        call pgpage
        call pgask(.false.)
345     continue  ! 345 isnt next line cos problems when i 'create'
        call pgvport(0.0,1.0,0.0,1.0)
        call clickopn(8,nmaxopts,0.03,ch1,opts,0)
        call pgeras
        if (ch1.eq.'1') choi='c'
        if (ch1.eq.'2') choi='d'
        if (ch1.eq.'3') choi='f'
        if (ch1.eq.'4') choi='z'
        if (ch1.eq.'5') choi='m'
        if (ch1.eq.'6') choi='s'
        if (ch1.eq.'7') choi='g'
        if (ch1.eq.'8') choi='q'
        call pgslct(id2)

c! ===============  circle associations or draw vectors =============

        if (choi.eq.'c') then
         starc=starc+1
         do i=1,npairs
          sizep=max(y42(i)-y41(i),x42(i)-x41(i))*5.0
          sizep=20.0
          call pgdrawellipse(0.5*(x41(i)+x42(i)),0.5*(y41(i)+y42(i)),
     /         sizep,sizep,0.0,starc,1,blacki)
         end do
        end if
        if (choi.eq.'d') then
         call pgsci(starc)
         do i=1,npairs
          if (x41(i).gt.x42(i)) then
           xd1=x42(i)
           xd2=x41(i)
           yd1=y42(i)
           yd2=y41(i)
          else
           xd1=x41(i)
           xd2=x42(i)
           yd1=y41(i)
           yd2=y42(i)
          end if
          l1=sqrt((y41(i)-y42(i))*(y41(i)-y42(i))+
     /       (x41(i)-x42(i))*(x41(i)-x42(i)))
          stfac=15.0
          l1=(stfac-1)/2.0*l1   !  (n-1)/2L so total is nL
          slop=(y42(i)-y41(i))/(x42(i)-x41(i))
          xp1=xd1-(l1/(1.0+slop*slop))
          xp2=xd2+(l1/(1.0+slop*slop))
          intr=yd2-xd2*slop
          yp1=slop*xp1+intr
          yp2=slop*xp2+intr
          call pgslw(3)
          call pgmove(xp1,yp1)
          call pgdraw(xp2,yp2)
          call pgslw(1)
          call pgsch(1.7)
          if (abs(xp1-x41(i)).lt.abs(xp1-x42(i))) then
           call pgarroww(xp1,yp1,xp2,yp2,slop,stfac*l1)
          else
           call pgarroww(xp2,yp2,xp1,yp1,slop,stfac*l1)
          end if
          call pgsch(1.0)
         end do
        end if
        call pgsci(1)
 
c! ===============  write out flux ratio or name =============
        if (choi.eq.'f') then
         call pgsci(2)
         call pgsch(1.2)
         do i=1,npairs
          xd1=var(1,i)/var(7,i)
          call pgnumb(int(xd1*10),-1,0,str2,nc)
          call pgtext(x41(i),y41(i),str2(1:nchar(str2)))
         end do
         call pgsch(1.0)
         call pgsci(1)
        end if

c! ===================== plot flux ratio as size of symbols
        if (choi.eq.'z') then
         mnf=var(1,1)/var(7,1)
         mxf=var(1,1)/var(7,1)
         do i=2,npairs
          if (var(1,i)/var(7,i).lt.mnf) mnf=var(1,i)/var(7,i)
          if (var(1,i)/var(7,i).gt.mxf) mxf=var(1,i)/var(7,i)
         end do
445      write (*,'(a,$)') '   Plot on (m)aster or (s)lave position ? '
         read (*,*) ch2
         if (ch2.ne.'m'.and.ch2.ne.'s') goto 445
         dm41=2.0*bm_pix(2)
         dm42=10.0*dm41
         mxf=6.0
         do i=1,npairs
          xd1=var(1,i)/var(7,i)
          dmin=(dm42-dm41)/(mxf-mnf)*xd1+(mxf*dm41-mnf*dm42)/(mxf-mnf)
          if (ch2.eq.'m') then
c           dmin=dmin*dcos(var(3,i)/rad)  ! why do i do this ?
           dmaj=dmin!*var(4,i)/var(5,i)
           dpa=0.0!var(6,i)
           call pgdrawellipse(x41(i),y41(i),dmaj,dmin,dpa,
     /          starc,1,blacki)
          else
c           dmin=dmin*dcos(var(9,i)/rad)
           dmaj=dmin!*var(10,i)/var(11,i)
           dpa=0.0!var(12,i)
           call pgdrawellipse(x42(i),y42(i),dmaj,dmin,dpa,
     /          starc,1,blacki)
          end if
         end do
        end if

c! =====================
        if (choi.eq.'m'.or.choi.eq.'s') then
457      write (*,'(a,$)') '   Enter star file name (*.gaul.star) : '
         read (*,*) starname
         extn='.gaul.star'
         if (.not.exists(starname,scratch,extn)) goto 457
         f2=starname(1:nchar(starname))//'.gaul.star'
         call getline(f2,scratch,nstar)
         f2=scratch(1:nchar(scratch))//f2(1:nchar(f2))
         call pgsci(blacki+1)
         call sub_overplotpasf_names(f2,nstar,choi,npairs,x41,y41,var,
     /        master,slave,scratch)
        end if

        if (choi.eq.'g') then
         call pgpage
         logged=.false.
         call sub_grey_drawimage(id2,hi,blc,trc,arr4,ii,jj,up,low,tr,
     /        lab,cod1,mn1,mx1,mn2,mx2,lowh,uph,dum4,colourscheme,
     /        str1,blacki,logged)
        end if

        if (choi.eq.'q') goto 334
        call pgslct(id3)
        goto 345
334     continue
        call pgslct(id3)
        call pgclos
c        call freewcs(wcs,wcslen)

        return
        end
c!
c!
c!
        subroutine sub_overplotpasf_names(f2,nstar,ms,npairs,
     /             x41,y41,var,fm,fs,scratch)
        implicit none
        integer nstar,npairs,i,ind,nchar,dumi,num,id,gid,flag
        character f2*500,nam(nstar)*30,ms,naam*10,fm*30,fs*30
        character fn*500,scratch*500,dumc30*30
        real*4 x41(npairs),y41(npairs)
        real*8 var(40,npairs)

        naam='         '
        call pgsch(0.6)
        if (ms.eq.'m') then
         fn=scratch(1:nchar(scratch))//fm(1:nchar(fm))//'.gaul.bin'
         ind=18
        end if
        if (ms.eq.'s') then
         fn=scratch(1:nchar(scratch))//fs(1:nchar(fs))//'.gaul.bin'
         ind=19
        end if
        do i=1,npairs
         open(unit=21,file=f2,status='old')
         id=int(var(ind,i))  ! id in gaul list of the source of the pair i
         open(unit=22,file=fn,status='old',form='unformatted')
333       read (22) gid,dumi,flag
          if (gid.lt.id) then
           if (flag.eq.0) read (21,777) dumc30
           goto 333
          else
           read (21,777) naam
          end if
         close(22)
         if (nchar(naam).gt.0) then     
           call pgtext(x41(i),y41(i),naam(1:nchar(naam)))
         end if
         close(21)
        end do
        call pgsch(1.0)
777     format(71x,a)

        return
        end
c!
c!
c!
        subroutine pgarroww(xp,yp,xpp,ypp,slop,d)
        implicit none
        real*4 xp,yp,slope(2),intr(2),d,f,xpp,ypp
        real*4 xx,xsn1,xsn2,slop,yy,ysn1,ysn2,cartdist
        integer i

        f=0.5
        slope(1)=(1.d0+slop)/(1.d0-slop)
        slope(2)=(slop-1.d0)/(1.d0+slop)
        do i=1,2
         intr(i)=yp-slope(i)*xp
         xsn1=xp+f*d/sqrt(1.0+slope(i)*slope(i))
         xsn2=xp-f*d/sqrt(1.0+slope(i)*slope(i))
         ysn1=slope(i)*xsn1+intr(i)
         ysn2=slope(i)*xsn2+intr(i)
         if (cartdist(xsn1,ysn1,xpp,ypp).lt.
     /       cartdist(xsn2,ysn2,xpp,ypp)) then
          xx=xsn1
          yy=ysn1
         else
          xx=xsn2
          yy=ysn2
         end if
         call pgmove(xp,yp)
         call pgdraw(xx,yy)
        end do


        return
        end


