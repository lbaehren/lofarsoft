c! take a f1.star file from somewhere (like read3cr.f in ~/cata/3C), assume same epoch coords (J2000.0)
c! and convert to gaul and srl of geometry of f2.
c! solnname of f1 is s2g, that is f1.s2g.gaul etc.
c! written so u can take 3c catalogue for example and do asrl with CS1 data.
c! CHANGE SRL FORMAT
c! have formatting for star file else problems with name (with spaces)

        subroutine star2gaulsrl(f1,f2,runcode,scratch,srldir,rcode)
        implicit none
        character f1*500,f2*500,runcode*2,scratch*500,srldir*500
        character fn1*500,extn*20,fn2*500,f3*500,keyword*500
        character keystrng*500,comment*500,code*1,rcode*1
        character ch1*1,wcsimp*1,calctot*1
        integer nchar,nstar,i,dumi,error,nn
        real*8 dumr,rms,freq

        extn='.star'
        fn1=f1(1:nchar(f1))//extn(1:nchar(extn))
        call getline(fn1,srldir,nstar)
        do i=nchar(f2),1,-1
         dumi=i
         if (f2(i:i).eq.'.') goto 333
        end do
333     if (i.eq.1) write (*,*) ' EERRRRORR in name '
        f3=f2(1:dumi-1)

        extn='.header'
        keyword='NAXIS'
        code='r'
        call get_keyword(f2,extn,keyword,keystrng,dumr,
     /             comment,code,scratch,error)
        nn=int(dumr)
        if (nn.eq.2) nn=3

        if (rcode.eq.'v') then
334      write (*,'(a,$)') '  Is this for (d)isplay or (a)ssociating ? '
         read (*,*) ch1
         if (ch1.ne.'a'.and.ch1.ne.'d') goto 334
         if (ch1.eq.'a') then
          write (*,'(a,$)') '  Enter typical rms of catalogue (Jy) : '
          read (*,*) rms
          if (rms.lt.0.d0) rms=0.d-3
          write (*,'(a,$)') '  Frequency of observation (MHz) : '
          read (*,*) freq
          freq=freq*1.d6
          if (freq.lt.0.d0) freq=50.d6
         end if
        else
         ch1='d'
        end if
        calctot='n'
335     write (*,'(a,$)') '  Is wcs for image important (y/n) ? : '
        read (*,*) wcsimp
        if (wcsimp.ne.'y'.and.wcsimp.ne.'n') goto 335

        call sub_star2gaulsrl(f1,f2,runcode,scratch,srldir,nstar,nn,
     /       rcode,ch1,rms,freq,calctot,wcsimp)
 
        return
        end
c!
c!
c!
        subroutine sub_star2gaulsrl(f1,f2,runcode,scratch,
     /        srldir,nstar,nn,rcode,ch1,rms,freq,calctot,wcsimp)
        implicit none
        include "constants.inc"
        include "wcs_bdsm.inc"
        integer nchar,nstar,hh,mm,dd,ma,dumi,error,i,nn
        character f1*500,f2*500,runcode*2,scratch*500,srldir*500
        character fn1*500,extn*20,fn2*500,sdec,ffmt*500,nam(nstar)*30
        real*8 ss,sa,flux(nstar),bmaj(nstar),bmin(nstar)
        real*8 bpa(nstar),ra(nstar),dec(nstar),a(6),x(nstar),y(nstar)
        real*8 crpix(nn),cdelt(nn),crval(nn),crota(nn),bm_pix(nn)
        character hdrfile*500,ctype(nn)*8,head*500,rmsmap*500,flag_s(1)
        character prog*500,f3*500,ch1*1,rcode*1,fn3*500,fn4*500
        character wcsimp*1,calctot*1
        integer n,m,nsrc,nffmt,nisl,gpi,flag(1),blc(2),trc(2),round
        real*8 deconv_s(3,1),e_amp(1),e_x0(1),e_y0(1),e_maj(1),e_min(1)
        real*8 e_pa(1),e_tot(1),ra1,dec1,rms,freq,beam,dumr
        real*8 dumr1(1),dumr2(1),dumr3(1),dumr4(1),dumr5(1),dumr6(1)
        integer wcslen,ifr,dumi_st
        parameter (wcslen=450)
        integer wcs(wcslen)

c! get astrometry of f2
        if (wcsimp.eq.'y') then
         fn2=f2(1:nchar(f2))//'.header'
         call read_head_coord(fn2,nn,ctype,crpix,cdelt,
     /         crval,crota,bm_pix,scratch)
         call wcs_struct(3,ctype,crpix,cdelt,crval,crota,wcs,wcslen)
         call sourcelistheaders(f2,fn1,n,m,nisl,nsrc,gpi,nffmt,
     /              ffmt,srldir)
        end if

c! get stars of f1
        beam=1.d99
        extn='.star'
        fn1=srldir(1:nchar(srldir))//f1(1:nchar(f1))//
     /        extn(1:nchar(extn))
        open(unit=21,file=fn1,status='old')
        do i=1,nstar
         read (21,777) hh,mm,ss,sdec,dd,ma,sa,bmaj(i),bmin(i),bpa(i),
     /        dumi,flux(i),nam(i)
         ra(i)=(hh+mm/60.d0+ss/3600.d0)*15.d0/rad  ! rad
         if (sdec.eq.'-') then
          dd=-dd
          ma=-ma
          sa=-sa
         end if
         call convertdecinv(dd,ma,sa,dec(i))
         dec(i)=dec(i)/rad                                  ! rad
         if (wcsimp.eq.'y') then
          call wcs_radec2xy(ra(i),dec(i),x(i),y(i),error,wcs,wcslen)
         else
          x(i)=ra(i)*rad
          y(i)=dec(i)*rad
         end if
         if (bmin(i).lt.beam) beam=bmin(i)  ! fw in asec
        end do
        close(21)
777     format(i2,1x,i2,1x,f6.3,1x,a1,i2,1x,i2,1x,f6.3,1x,f9.4,1x,
     /         f9.4,1x,f7.2,1x,i2,1x,f13.7,1x,a30)  ! from writeaipsfiles.f + nam(i)
778     format(i2,1x,i2,1x,f6.3,1x,a1,i1,i1,1x,i2,1x,f6.3,1x,f9.4,1x,
     /         f9.4,1x,f7.2,1x,i2,1x,f13.7,1x,a10)  ! from writeaipsfiles.f + nam(i)

c! copy most of code below from cr8sources.f
        fn1=srldir(1:nchar(srldir))//f1(1:nchar(f1))//'.s2g.gaul'
        fn2=srldir(1:nchar(srldir))//f1(1:nchar(f1))//'.s2g.gaul.bin'
        fn3=srldir(1:nchar(srldir))//f1(1:nchar(f1))//'.star'
        fn4=srldir(1:nchar(srldir))//f1(1:nchar(f1))//'.s2g.gaul.star'
        open(unit=28,file=fn1(1:nchar(fn1)),status='unknown')
        open(unit=29,file=fn2(1:nchar(fn2)),form='unformatted')
        open(unit=30,file=fn3(1:nchar(fn3)),form='formatted')
        open(unit=31,file=fn4(1:nchar(fn4)),form='formatted')
        prog='s2g.f'
        rmsmap='const'
        call sub_sourcemeasure_writehead(28,f1,f1,n,m,nstar,
     /       1.d0,prog,rmsmap,5.d0)
        if (ch1.eq.'a') then
         if (ctype(3)(1:4).eq.'FREQ') then
          ifr=3        
         else
          if (ctype(4)(1:4).eq.'FREQ') then
           ifr=4
          else
           ifr=3
           ctype(3)='FREQ-OBS' 
          end if
         end if
         crpix(ifr)=1.d0
         crval(ifr)=freq
        end if
        call writefitshead(f1,nn,crpix,ctype(1),ctype(2),ctype(3),
     /       cdelt,crval,crota,bm_pix,scratch) ! write to header file
        f3=f1(1:nchar(f1))//'.s2g'
        call writefitshead(f3,nn,crpix,ctype(1),ctype(2),ctype(3),
     /       cdelt,crval,crota,bm_pix,scratch) ! write to header file

        if (ch1.eq.'a') then  ! not sure if this will be a problem
         cdelt(1)=beam/4.d0/3600.d0
         cdelt(2)=beam/4.d0/3600.d0
        end if

        dumi=0
        do i=1,nstar
         a(1)=flux(i)
         a(2)=x(i)
         a(3)=y(i)
         a(4)=bmaj(i)/(fwsig*abs(cdelt(1))*3600.d0)
         a(5)=bmin(i)/(fwsig*abs(cdelt(2))*3600.d0)
         if (bpa(i).le.90.d0) then 
          a(6)=bpa(i)+90.d0
         else
          if (bpa(i).gt.90.d0.and.bpa(i).le.180.d0) then 
           a(6)=bpa(i)-90.d0
          end if
         end if
         deconv_s(1,1)=0.d0
         deconv_s(2,1)=0.d0
         deconv_s(3,1)=0.d0
         flag(1)=0
         flag_s(1)=' '
         e_amp(1)=rms
         e_x0(1)=0.d0
         e_y0(1)=0.d0
         e_maj(1)=0.d0
         e_min(1)=0.d0
         e_pa(1)=0.d0
         e_tot(1)=rms
         dumr1(1)=0.d0
         dumr2(1)=0.d0
         dumr3(1)=0.d0
         dumr4(1)=0.d0
         dumr5(1)=0.d0
         dumr6(1)=a(1)
         if (ch1.eq.'a') then
          dumr=pi/8.d0/dlog(2.d0)*bmaj(i)*bmin(i)*a(1)*a(1)/rms/rms
          dumr=2.d0/dumr
          e_x0(1)=sqrt(dumr*bmaj(i)*bmaj(i)/8.d0/dlog(2.d0))
          e_y0(1)=sqrt(dumr*bmin(i)*bmin(i)/8.d0/dlog(2.d0))
          e_maj(1)=sqrt(dumr*bmaj(i))
          e_min(1)=sqrt(dumr*bmin(i))
          e_pa(1)=1.d0
         end if
c! if source is below horizon and other funny problems.
         if (wcsimp.eq.'y') then
          call wcs_xy2radec(x(i),y(i),ra1,dec1,error,wcs,wcslen)
          call correctrarad(ra1)
          if (x(i).lt.0.d0.or.x(i).gt.n*1.d0.or.
     /        y(i).lt.0.d0.or.y(i).gt.m*1.d0) flag(1)=1
          if (abs(ra1-ra(i)).gt.5.d0/3600.d0/rad.or.
     /        abs(dec1-dec(i)).gt.5.d0/3600.d0/rad) flag(1)=2
         end if
         blc(1)=round(x(i))-bm_pix(1)*2
         blc(2)=round(y(i))-bm_pix(1)*2
         trc(1)=round(x(i))+bm_pix(1)*2
         trc(2)=round(y(i))+bm_pix(1)*2
         call putin_srclist(28,29,i,dumi,a,1,6,1,0.d0,0.d0,0.d0,0.d0,
     /     0.d0,0.d0,0,0,ffmt,f1,ctype,crpix,cdelt,crval,crota,
     /     deconv_s,0,flag,flag_s,e_amp,e_x0,e_y0,e_maj,e_min,e_pa,
     /     e_maj,e_min,e_pa,e_tot,scratch,bm_pix,.false.,wcs,wcslen,
     /     blc,trc,dumr1,dumr2,dumr3,dumr4,dumr5,dumr6,wcsimp,calctot)
         read (30,777) hh,mm,ss,sdec,dd,ma,sa,bmaj(1),bmin(1),bpa(1),
     /        dumi_st,flux(1),nam(1)
         if (flag(1).eq.0) then
          if (abs(dd).ge.10.d0) then
           write (31,777) hh,mm,ss,sdec,dd,ma,sa,
     /        bmaj(1),bmin(1),bpa(1),dumi_st,flux(1),nam(1)
          else
           write (31,778) hh,mm,ss,sdec,0,dd,ma,sa,
     /        bmaj(1),bmin(1),bpa(1),dumi_st,flux(1),nam(1)
          end if
         end if
        end do
        close(28)
        close(29)
        call sub_sourcemeasure_writeheadmore(nstar,1,fn1,ffmt,scratch)
        
        fn1=srldir(1:nchar(srldir))//f1(1:nchar(f1))//'.s2g.srl'
        fn2=srldir(1:nchar(srldir))//f1(1:nchar(f1))//'.s2g.srl.bin'
        open(unit=28,file=fn1(1:nchar(fn1)),form='formatted') 
        open(unit=29,file=fn2(1:nchar(fn2)),form='unformatted') 
        call putsrlhead(28,nstar,f1,1,n,m)
        do i=1,nstar
         write (28,888) i,i,i,'S   '
         write (29) i,i,i,'S   '
        end do
        close(28)
        close(29)
        close(30)
        close(31)
888     format(3(i5,1x),a4,1x,6(1Pe11.3,1x),8(0Pf13.9,1x),
     /         12(0Pf10.5,1x),1(1Pe11.3,1x),i4)
 

c! copy filename.star to filename.s2g.gaul.star for completeness (and for asrl)
c! adding gaul later, for consistency for other programs. this is what
c! we will use hereafter
c! modified. will only write flag=0 sources.
c! and moved it up.

        return
        end




