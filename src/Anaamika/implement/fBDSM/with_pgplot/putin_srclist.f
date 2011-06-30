c! write the sourcelist
c! CHANGE GAUL FORMAT
c! if ch2='n' then wcs doesnt matter, ra,dec is in x,y
c! if calctot is no then take tot from dumr6 rather than peak. for now.


        subroutine putin_srclist(nn,nnb,ii,nsrc,a,max_msrc,max_msrc6,
     /   nmulsrc,rstd,rav,sstd,sav,chisq,q,delx,dely,ffmt,f1,ctype,crpix
     /   ,cdelt,crval,crota,deconv_s,error,flag,flag_s,e_amp,e_x0,
     /   e_y0,e_maj,e_min,e_pa,e_d1,e_d2,e_d3,e_tot,scratch,
     /   bm_pix,realsrl,wcs,wcslen,blc,trc,dumr1,dumr2,dumr3,dumr4,
     /   dumr5,dumr6,wcsimp,calctot)
        implicit none
        include "constants.inc"
        integer nn,ii,nsrc,max_msrc6,nmulsrc,i,delx,dely,d,nchar,error
        integer hh,mm,dd,ma,max_msrc,error1,flag(max_msrc),nnb,srcid
        integer wcslen,wcs(wcslen),blc(2),trc(2)
        real*8 chisq,q,a(max_msrc6),ra,dec,dra,ddec,ss,sa,e_ra,e_dec
        real*8 crpix(3),cdelt(3),crval(3),crota(3),rstd,rav
        real*8 sstd(max_msrc),sav(max_msrc),deconv_s(3,max_msrc),d1,d2
        real*8 e_amp(max_msrc),e_x0(max_msrc),e_y0(max_msrc)
        real*8 e_maj(max_msrc),e_min(max_msrc),e_pa(max_msrc)
        real*8 e_d1(max_msrc),e_d2(max_msrc),e_d3(max_msrc)
        real*8 e_tot(max_msrc),tot,bm_pix(3)
        real*8 dumr1(max_msrc),dumr2(max_msrc),dumr3(max_msrc)
        real*8 dumr4(max_msrc),dumr5(max_msrc),dumr6(max_msrc)
        character ffmt*500,f1*500,s,strcoord*500,wcsimp*1
        character ctype(3)*8,flag_s(max_msrc)*5,scratch*500
        character calctot*1
        logical realsrl

        d1=fwsig*abs(cdelt(1))*3600.d0
        d2=fwsig*abs(cdelt(2))*3600.d0
        ffmt="(3(i7,1x),4(1Pe11.3,1x),4(0Pf13.9,1x),"//!a42,1x,"//
     /      "16(0Pf11.5,1x),4(1Pe11.3,1x),0Pf7.3,1x,0Pf7.3,1x,i7,"
     /       //"4(1x,i5),6(1x,1Pe11.3))"
        do 100 i=1,nmulsrc
         d=6*(i-1)
         if (flag(i).ne.0) then
          call maxit(flag_s(i),d,a,max_msrc6,delx,dely,d1,d2)
         end if
         nsrc=nsrc+1
         if (realsrl.eqv..true.) then
          srcid=0 
         else
          srcid=nsrc
         end if
         if (wcsimp.eq.'y') then
          call wcs_xy2radec(a(2+d)+delx,a(3+d)+dely,ra,dec,error1,
     /         wcs,wcslen)
          if (error1.ne.0.and.flag(i).eq.0) write (*,*) ' ERRORRR !!!! '
     /        ,a(2+d)+delx,a(3+d)+dely
          call correctrarad(ra)
          dra=ra*rad
          ddec=dec*rad
          e_ra=e_x0(i)*d1/fwsig/15.d0/3600.d0
          e_dec=e_y0(i)*d2/fwsig/3600.d0
         else
          dra=a(2+d)
          ddec=a(3+d)
          e_ra=e_x0(i)
          e_dec=e_y0(i)
         end if

         if (calctot.eq.'y') then
          tot=a(1+d)*(a(4+d)*a(5+d)*fwsig*fwsig)/     ! a(4) = sig_pix
     /        (bm_pix(1)*bm_pix(2))
          if (tot.lt.a(1+d)) tot=a(1+d)
         else
          tot=dumr6(i)
         end if
c         e_tot(i)=e_amp(i)*sqrt(tot/a(1+d))

         write (nn,ffmt) nsrc,ii,flag(i),tot,e_tot(i),
     /    a(1+d),e_amp(i),dra,e_ra,ddec,e_dec,
     /    a(2+d)+delx,e_x0(i),a(3+d)+dely,e_y0(i),a(4+d)*d1,e_maj(i)*d1
     /    ,a(5+d)*d2,e_min(i)*d2,a(6+d),e_pa(i),deconv_s(1,i)*d1,
c     /    e_d1(i)*d1,deconv_s(2,i)*d2,e_d2(i)*d2,deconv_s(3,i),e_d3(i),
     /   e_maj(i)*d1,deconv_s(2,i)*d2,e_min(i)*d2,deconv_s(3,i),e_pa(i),
     /   sstd(i),sav(i),rstd,rav,-99.999d0,-99.999d0,srcid,blc(1),
     /   blc(2),trc(1),trc(2),dumr1(i),dumr2(i),dumr3(i),dumr4(i),
     /   dumr5(i),dumr6(i)
         write (nnb) nsrc,ii,flag(i),tot,e_tot(i),a(1+d),e_amp(i),
     /    dra,e_x0(i)*d1/fwsig/15.d0,ddec,e_y0(i)*d2/fwsig,
     /    a(2+d)+delx,e_x0(i),a(3+d)+dely,e_y0(i),a(4+d)*d1,e_maj(i)*d1
     /    ,a(5+d)*d2,e_min(i)*d2,a(6+d),e_pa(i),deconv_s(1,i)*d1,
c     /    e_d1(i)*d1,deconv_s(2,i)*d2,e_d2(i)*d2,deconv_s(3,i),
c     /    e_d3(i),sstd(i),sav(i),rstd,rav,chisq,q,0 
     /    e_maj(i)*d1,deconv_s(2,i)*d2,e_min(i)*d2,deconv_s(3,i),
     /    e_pa(i),sstd(i),sav(i),rstd,rav,-99.999d0,-99.999d0,srcid,
     /    blc(1),blc(2),trc(1),trc(2),dumr1(i),dumr2(i),dumr3(i),
     /    dumr4(i),dumr5(i),dumr6(i)
100     continue

        return
        end


