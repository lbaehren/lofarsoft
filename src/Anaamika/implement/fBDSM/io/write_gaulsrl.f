c! modified star2gaulsrl so that i can directly pass all the column
c! values instead of providing default options for most of them.
c! This is for cases where you dont need xpix,ypix and dont need
c! to copy wcs from an image. For example, an external catalogue which
c! has more entries than just a star file, like true errors.
c! Done first for NVSScat_condon.
c!
c! Instead of s2g, we use c2g for catalogue to gaul.
c! input ra, dec in deg; bmaj etc in fw_asec; flux in Jy
c! CHANGE GAUL SRL FORMAT

        subroutine write_gaulsrl(f1,runcode,scratch,srldir,ngau,
     /   peak,epeak,tot,etot,ra,era,dec,edec,bmaj,ebmaj,bmin,ebmin,bpa,
     /   ebpa,dbmaj,edbmaj,dbmin,edbmin,dbpa,edbpa,rstd,rav,sstd,sav,
     /   spin,espin,imrms,dumr2,dumr3,dumr4,dumr5,dumr6,cbmaj,cbmin,
     /   cbpa,freq,catrms,nam)
        implicit none
        include "constants.inc"
        character f1*500,runcode*2,scratch*500,srldir*500,ffmt*500
        character ctype(3)*8,prog*20,rmsmap*500,f2*500,wcsimp,calctot
        character fn1*500,fn2*500,fn3*500,fn4*500,fn5*500
        integer i,ngau,nchar,nn,dumic,hh,mm,dd,ma,blc(2),trc(2),n,m
        character s*1,nam(ngau)*500,sflag_s(1)*1
        integer sflag(1)
        real*8 peak(ngau),epeak(ngau),tot(ngau),etot(ngau),ra(ngau)
        real*8 era(ngau),dec(ngau),edec(ngau),bmaj(ngau),ebmaj(ngau)
        real*8 bmin(ngau),ebmin(ngau),bpa(ngau),ebpa(ngau),dbmaj(ngau)
        real*8 edbmaj(ngau),dbmin(ngau),edbmin(ngau),dbpa(ngau)
        real*8 edbpa(ngau),rstd(ngau),rav(ngau),sstd(ngau),sav(ngau)
        real*8 spin(ngau),espin(ngau),imrms(ngau),dumr2(ngau)
        real*8 dumr3(ngau),dumr4(ngau),dumr5(ngau),dumr6(ngau)
        real*8 cbmaj,cbmin,cbpa,freq,catrms,bm_pix(3),sa,ss
        real*8 crpix(3),cdelt(3),crval(3),crota(3),a(6),deconv_s(3,1)
        real*8 speak(1),sepeak(1),stot(1),setot(1),sra(1)
        real*8 sera(1),sdec(1),sedec(1),sbmaj(1)
        real*8 sbmin(1),sebmin(1),sbpa(1),sebpa(1)
        real*8 sebmaj(1),sdbmaj(1),e_x0(1),e_y0(1),se_x0(1),se_y0(1)
        real*8 sedbmaj(1),sdbmin(1),sedbmin(1),sdbpa(1)
        real*8 sedbpa(1),srstd(1),srav(1),ssstd(1),ssav(1)
        real*8 sspin(1),sespin(1),simrms(1),sdumr2(1)
        real*8 sdumr3(1),sdumr4(1),sdumr5(1),sdumr6(1)
        integer wcslen
        parameter (wcslen=450)
        integer wcs(wcslen)

        data ctype/'RA---SIN','DEC--SIN','FREQ'/
        data crpix/1.d0,1.d0,1.d0/
        data crval/180.d0,90.d0,0.d0/   ! crval(3) is dummy, fill later
        data crota/0.d0,0.d0,0.d0/
        data bm_pix/4.d0,4.d0,1.d0/
        data wcs/450*0/

        wcsimp='n'
        calctot='n'
        crval(3)=freq
        cdelt(1)=abs(cbmaj/bm_pix(1))
        cdelt(2)=abs(cbmin/bm_pix(2))
        cdelt(3)=freq/100.d0

        n=3000
        m=3000

        fn1=srldir(1:nchar(srldir))//f1(1:nchar(f1))//'.c2g.gaul'
        fn2=srldir(1:nchar(srldir))//f1(1:nchar(f1))//'.c2g.gaul.bin'
        fn3=srldir(1:nchar(srldir))//f1(1:nchar(f1))//'.star'
        fn4=srldir(1:nchar(srldir))//f1(1:nchar(f1))//'.c2g.gaul.star'
        fn5=srldir(1:nchar(srldir))//f1(1:nchar(f1))//'.c2g.csv'
        open(unit=28,file=fn1(1:nchar(fn1)),status='unknown')
        open(unit=29,file=fn2(1:nchar(fn2)),form='unformatted')
        open(unit=30,file=fn3(1:nchar(fn3)),form='formatted')
        open(unit=31,file=fn4(1:nchar(fn4)),form='formatted')
        open(unit=32,file=fn5(1:nchar(fn5)),form='formatted')
        write (32,*) '_RAJ2000,_DEJ2000'
        prog='write_gaulsrl.f'
        rmsmap='map'
        call sub_sourcemeasure_writehead(28,f1,f1,n,m,ngau,
     /       catrms,prog,rmsmap,5.d0)
        nn=3   ! NAXES
        call writefitshead(f1,nn,crpix,ctype(1),ctype(2),ctype(3),
     /       cdelt,crval,crota,bm_pix,scratch) ! write to header file
        f2=f1(1:nchar(f1))//'.c2g'
        call writefitshead(f2,nn,crpix,ctype(1),ctype(2),ctype(3),
     /       cdelt,crval,crota,bm_pix,scratch) ! write to header file

        dumic=0
        do i=1,ngau
         a(1)=peak(i)
         a(2)=ra(i)
         a(3)=dec(i)
         e_x0(1)=era(i)
         e_y0(1)=edec(i)
         a(4)=bmaj(i)/(fwsig*abs(cdelt(1))*3600.d0)
         a(5)=bmin(i)/(fwsig*abs(cdelt(2))*3600.d0)
         ebmaj(i)=ebmaj(i)/(fwsig*abs(cdelt(1))*3600.d0)
         ebmin(i)=ebmin(i)/(fwsig*abs(cdelt(2))*3600.d0)
         edbmaj(i)=edbmaj(i)/(fwsig*abs(cdelt(1))*3600.d0)
         edbmin(i)=edbmin(i)/(fwsig*abs(cdelt(2))*3600.d0)
         if (bpa(i).le.90.d0) then 
          a(6)=bpa(i)+90.d0
         else
          if (bpa(i).gt.90.d0.and.bpa(i).le.180.d0) then 
           a(6)=bpa(i)-90.d0
          end if
         end if
         deconv_s(1,1)=dbmaj(i)/(fwsig*abs(cdelt(1))*3600.d0)
         deconv_s(2,1)=dbmin(i)/(fwsig*abs(cdelt(1))*3600.d0)
         if (dbpa(i).le.90.d0) then 
          deconv_s(3,1)=dbpa(i)+90.d0
         else
          if (dbpa(i).gt.90.d0.and.dbpa(i).le.180.d0) then 
           deconv_s(3,1)=dbpa(i)-90.d0
          end if
         end if
         blc(1)=1
         blc(2)=1
         trc(1)=100
         trc(2)=100

         srstd(1)=rstd(i)
         srav(1)=rav(i)
         ssstd(1)=sstd(i)
         ssav(1)=sav(i)
         sspin(1)=spin(i)
         sespin(1)=espin(i)
         sflag(1)=0
         sflag_s(1)=' '
         sepeak(1)=epeak(i)
         se_x0(1)=e_x0(1)
         se_y0(1)=e_y0(1)
         sebmaj(1)=ebmaj(i)
         sebmin(1)=ebmin(i)
         sebpa(1)=ebpa(i)
         sedbmaj(1)=edbmaj(i)
         sedbmin(1)=edbmin(i)
         sedbpa(1)=edbpa(i)
         setot(1)=etot(i)
         simrms(1)=imrms(i)
         sdumr2(1)=dumr2(i)
         sdumr3(1)=dumr3(i)
         sdumr4(1)=dumr4(i)
         sdumr5(1)=dumr5(i)
         sdumr6(1)=tot(i)
         stot(1)=tot(i)
         
         call putin_srclist(28,29,i,dumic,a,1,6,1,srstd(1),srav(1),
     /     ssstd(1),ssav(1),sspin(1),sespin(1),100,100,ffmt,f1,ctype,
     /     crpix,
     /     cdelt,crval,crota,deconv_s,0,sflag(1),sflag_s(1),sepeak(1),
     /     se_x0(1),se_y0(1),sebmaj(1),sebmin(1),sebpa(1),sedbmaj(1),
     /     sedbmin(1),sedbpa(1),setot(1),scratch,bm_pix,.false.,
     /     wcs,wcslen,blc,trc,simrms(1),sdumr2(1),sdumr3(1),sdumr4(1),
     /     sdumr5(1),sdumr6(1),wcsimp,calctot)

         call convertra(ra(i),hh,mm,ss)
         call convertdec(dec(i),s,dd,ma,sa)
         if (bmaj(i).gt.9999.9d0) bmaj(i)=9999.9d0
         if (bmin(i).gt.9999.9d0) bmin(i)=9999.9d0
         if (abs(dd).ge.10.d0) then
          write (31,777) hh,mm,ss,s,dd,ma,sa,
     /       bmaj(i),bmin(i),bpa(i),4,tot(i),nam(i)(1:30)
         else
          write (31,778) hh,mm,ss,s,0,dd,ma,sa,
     /       bmaj(i),bmin(i),bpa(i),4,tot(i),nam(i)(1:30)
         end if
         write (32,'(f9.5,a1,f9.5)') ra(i)*rad,',',dec(i)*rad
        end do
        close(28)
        close(29)
        call sub_sourcemeasure_writeheadmore(ngau,1,fn1,ffmt,scratch)
        
        fn1=srldir(1:nchar(srldir))//f1(1:nchar(f1))//'.c2g.srl'
        fn2=srldir(1:nchar(srldir))//f1(1:nchar(f1))//'.c2g.srl.bin'
        open(unit=28,file=fn1(1:nchar(fn1)),form='formatted') 
        open(unit=29,file=fn2(1:nchar(fn2)),form='unformatted') 
        call putsrlhead(28,ngau,f1,1,n,m)
        do i=1,ngau
         write (28,888) i,i,i,'S   '
         write (29) i,i,i,'S   '
        end do
        close(28)
        close(29)
        close(30)
        close(31)
        close(32)
777     format(i2,1x,i2,1x,f6.3,1x,a1,i2,1x,i2,1x,f6.3,1x,f9.4,1x,
     /         f9.4,1x,f7.2,1x,i2,1x,f13.7,1x,a30)  
778     format(i2,1x,i2,1x,f6.3,1x,a1,i1,i1,1x,i2,1x,f6.3,1x,f9.4,1x,
     /         f9.4,1x,f7.2,1x,i2,1x,f13.7,1x,a10) 
888     format(3(i7,1x),a4,1x,6(1Pe11.3,1x),8(0Pf13.9,1x),
     /         12(0Pf10.5,1x),1(1Pe11.3,1x),i4)
 
        return
        end




