c! fits an island with multiple gaussians.
c! writes resultant gaussians to a .gaul file.
c! CHANGE GAUL FORMAT

        subroutine fit_island(subim,subrms,f1,xpix,ypix,npix_isl,maxmem,
     /     blc,trc,av,subn,subm,rstd,rav,max_msrc,max_msrc6,nn,nnb,
     /     ii,nsrc,ffmt,ctype,crpix,cdelt,crval,crota,bm_pix,delx,
     /     dely,gauperisl,scratch,thresh_pix,avclip)
        implicit none
        integer max_msrc,max_msrc6,nn,ii,nnb
        integer n,m,npix_isl,maxmem,blc(2),trc(2),gauperisl
        integer xpix(maxmem),ypix(maxmem),i,j,subn,subm
        real*8 pkv(max_msrc),a(max_msrc6),bmsampl
        character f1*(*),fn*500,hist*10,ffmt*(*)
        integer mask(subn,subm),delx,dely,error
        real*8 rmask(subn,subm),bm_pix(3),deconv_s(3,max_msrc)
        real*8 sstd(max_msrc),sav(max_msrc)
        integer nmulsrc,ia(max_msrc6),nsrc,nmul6
        integer xpk(max_msrc),ypk(max_msrc),flag(max_msrc)
        real*8 chisq,redchi,q,resid(subn,subm)
        real*8 subim(subn,subm),av,rstd,rav,thresh_pix
        real*8 crpix(3),cdelt(3),crval(3),crota(3),subrms(subn,subm)
        real*8 e_amp(max_msrc),e_x0(max_msrc),e_y0(max_msrc)
        real*8 e_maj(max_msrc),e_min(max_msrc),e_pa(max_msrc)
        real*8 e_tot(max_msrc),keyvalue,avclip
        character ctype(3)*8,flag_s(max_msrc),dir*500,fg*500,scratch*500
        character extn*10,keyword*500,keystrng*500,comment*500

        do i=1,max_msrc6
         ia(i)=1
        end do
        
c! prepare the mask
        call initialisemask(mask,subn,subm,subn,subm,0)
        do i=1,npix_isl
         mask(xpix(i)-delx,ypix(i)-dely)=1
        end do

c! get number of multiple distinct sources. 
c! get_mulsrc1 finds number of peaks (highest in 8-connective region inside island.
c! get_mulsrc2 'CLEANs' with beam upto some rms to find number of sources to fit.
c! shud choose based on residual rms.
c        call get_mulsrc1(subim,subrms,subn,subm,mask,nmulsrc,
c     /                  xpk,ypk,pkv,max_msrc,f1,thresh_pix,scratch,avclip)


c        call get_mulsrc2(subim,subrms,subn,subm,mask,nmulsrc,
c     /                  xpk,ypk,pkv,max_msrc,f1,bm_pix,thresh_pix)

        call get_mulsrc3(subim,subrms,subn,subm,mask,nmulsrc,
     /     xpk,ypk,pkv,max_msrc,f1,bm_pix,thresh_pix,scratch,avclip)

        call put_iniparm_mulgaus(nmulsrc,max_msrc,max_msrc6,a,pkv,xpk,
     /       ypk,bm_pix)
         
        nmul6=nmulsrc*6
        call fit_mulgauss(subim,subrms,subn,subm,mask,nmulsrc,
     /       nmul6,max_msrc,max_msrc6,a,ia,chisq,redchi,q,resid,
     /       rstd,rav,delx,dely,ii,sstd,sav,cdelt,bm_pix,deconv_s,error
     /       ,thresh_pix,av,flag,flag_s,e_amp,e_x0,e_y0,e_maj,e_min,
     /       e_pa,e_tot)

        if (nmulsrc.gt.gauperisl) gauperisl=nmulsrc

        call putin_srclist(nn,nnb,ii,nsrc,a,max_msrc,max_msrc6,nmulsrc,
     /    rstd,rav,sstd,sav,chisq,q,delx,dely,ffmt,f1,ctype,crpix,cdelt,
     /    crval,crota,deconv_s,error,flag,flag_s,e_amp,e_x0,e_y0,
     /    e_maj,e_min,e_pa,e_tot,scratch,bm_pix)

        return
        end

c! -----------------------------------------------------------------------------------
c! this tries to find number of sources to fit inside one island by == number of
c! peaks inside the island (downward gradient in 8-conectivity. 

        subroutine get_mulsrc1(subim,subrms,subn,subm,mask,nmulsrc,
     /         xmsrc,ymsrc,pkv,max_msrc,f1,thresh_pix,scratch,avclip)
        implicit none
        integer subn,subm,nmulsrc,max_msrc,mask(subn,subm)
        integer xmsrc(max_msrc),ymsrc(max_msrc),i,j,coord
        integer xsurr(8),ysurr(8),fac,i1,ndigit
        real*8 subim(subn,subm),pkv(max_msrc),keyvalue
        real*8 subrms(subn,subm),thresh_pix,avclip
        character f1*500,extn*10,keyword*500,keystrng*500
        character comment*500,dir*500,fg*500,scratch*500
        logical okay,pixinimage

        ndigit=int(log10(max(subn,subm)*1.0))+1      ! number of digits in max(n,m)
        fac=int(10**ndigit)
        nmulsrc=0
        do 100 i=1,subn
         do 110 j=1,subm
          if (subim(i,j)-avclip.ge.thresh_pix*subrms(i,j)) then
           coord=j*fac+i
           call getsurr_8pix(coord,xsurr,ysurr,fac) 
           okay=.true.
           do i1=1,8
            if (pixinimage(xsurr(i1),ysurr(i1),1,subn,1,subm)) then
             okay=okay.and.(subim(i,j).gt.subim(xsurr(i1),ysurr(i1)))
            end if
           end do
           if (okay) then
            nmulsrc=nmulsrc+1
            xmsrc(nmulsrc)=i
            ymsrc(nmulsrc)=j
            pkv(nmulsrc)=subim(i,j)
           end if
          end if
110      continue
100     continue
        if (nmulsrc.gt.max_msrc) write (*,*) '   ### ERROR !!!! ',
     /       nmulsrc,max_msrc

        return
        end
c! -----------------------------------------------------------------------------------
c! this tries to find number of sources to fit inside one island by == CLEANing
c! down to some level with psf and counting number of iterations to go to some rms

        subroutine get_mulsrc2(subim,subrms,subn,subm,mask,nmulsrc,
     /             xmsrc,ymsrc,pkv,max_msrc,f1,bm_pix,thresh_pix)
        implicit none
        integer subn,subm,nmulsrc,max_msrc,mask(subn,subm),nchar
        integer xmsrc(max_msrc),ymsrc(max_msrc),i,j,xmax,ymax,round
        real*8 subim(subn,subm),pkv(max_msrc),rx,ry
        real*8 subrms(subn,subm),maxv,im(subn,subm),thresh_pix
        real*8 stdnew,avnew,std,av,bm_pix(3),stdold,avold,m1
        character f1*500,f2*500,getchar

        nmulsrc=0
        call matav(subrms,subn,subm,subn,subm,std)
        call copyarray(subim,subn,subm,subn,subm,1,1,subn,subm,im)
        call arrstatmask(im,mask,subn,subm,1,1,subn,subm,stdnew,avnew)

333     continue
        call maxarraymask(im,mask,subn,subm,xmax,ymax,maxv)
        rx=0.d0
        ry=0.d0
        do 100 i=max(1,xmax-3),min(subn,xmax+3)
         do 110 j=max(1,ymax-3),min(subm,ymax+3)
          if (im(i,j).ge.2.d0*std) then
           m1=m1+im(i,j)
           rx=rx+im(i,j)*i
           ry=ry+im(i,j)*j
          end if
110      continue
100     continue
        rx=rx/m1
        ry=ry/m1

        nmulsrc=nmulsrc+1
        xmsrc(nmulsrc)=round(rx)
        ymsrc(nmulsrc)=round(ry)
        pkv(nmulsrc)=im(xmsrc(nmulsrc),ymsrc(nmulsrc))

        call clean_sub(im,subn,subm,rx,ry,pkv(nmulsrc),bm_pix)
        stdold=stdnew
        avold=avnew
        call arrstatmask(im,mask,subn,subm,1,1,subn,subm,stdnew,avnew)
        if (stdnew.gt.std*1.1d0.and.stdold/stdnew.gt.1.1d0) goto 333

        return
        end
c!
c! ---------------
c! combines mulsrc1 and mulsrc2. first get multiple peaks, then clean, then look at residue.
        subroutine get_mulsrc3(subim,subrms,subn,subm,mask,nmulsrc,
     /             xmsrc,ymsrc,pkv,max_msrc,f1,bm_pix,thresh_pix,
     /             scratch,avclip)
        implicit none
        integer subn,subm,nmulsrc,max_msrc,mask(subn,subm)
        integer xmsrc(max_msrc),ymsrc(max_msrc),i,j,coord
        integer xsurr(8),ysurr(8),fac,i1,ndigit,xmax,ymax,nchar
        real*8 subim(subn,subm),pkv(max_msrc),im(subn,subm)
        real*8 subrms(subn,subm),thresh_pix,avclip,bm_pix(3)
        real*8 std,stdnew,avnew,maxv,distbm,dumr,keyvalue
        character f1*500,extn*10,keyword*500,keystrng*500,comment*500
        character getchar,f2*500,dir*500,fg*500,scratch*500
        logical okay,pixinimage,more

        call matav(subrms,subn,subm,subn,subm,std)
        call copyarray(subim,subn,subm,subn,subm,1,1,subn,subm,im)

        ndigit=int(log10(max(subn,subm)*1.0))+1      ! number of digits in max(n,m)
        fac=int(10**ndigit)
        nmulsrc=0
        do 100 i=1,subn
         do 110 j=1,subm
          if (subim(i,j)-avclip.ge.thresh_pix*subrms(i,j)) then
           coord=j*fac+i
           call getsurr_8pix(coord,xsurr,ysurr,fac) 
           okay=.true.
           do i1=1,8
            if (pixinimage(xsurr(i1),ysurr(i1),1,subn,1,subm)) then
             okay=okay.and.(subim(i,j).gt.subim(xsurr(i1),ysurr(i1)))
            end if
           end do
           if (okay) then
            nmulsrc=nmulsrc+1
            xmsrc(nmulsrc)=i
            ymsrc(nmulsrc)=j
            pkv(nmulsrc)=subim(i,j)
            call clean_sub(im,subn,subm,i*1.d0,j*1.d0,subim(i,j),bm_pix)
           end if
          end if
110      continue
100     continue
        if (nmulsrc.gt.max_msrc) write (*,*) '   ### ERROR !!!! ',
     /       nmulsrc,max_msrc

333     continue                                ! now to check residue
        more=.false.
        call arrstatmask(im,mask,subn,subm,1,1,subn,subm,stdnew,avnew)
        call maxarraymask(im,mask,subn,subm,xmax,ymax,maxv)
        if (stdnew.gt.std.and.maxv.gt.0.d0) then     ! first cut
         if (maxv-avclip.ge.thresh_pix*std           ! res peak is real
     /       .and.maxv-avclip.ge.3.d0*stdnew) then
          more=.true.
          do i=1,nmulsrc
           dumr=sqrt((xmax-xmsrc(i))*(xmax-xmsrc(i))+(ymax-ymsrc(i))*
     /            (ymax-ymsrc(i))*1.d0)
           distbm=dumr/sqrt(bm_pix(1)*bm_pix(2))   ! beam size to be redone
           if (distbm.lt.0.5d0.or.dumr.lt.2.2d0) more=.false.    ! > sqrt5
          end do
          if (more) then
            nmulsrc=nmulsrc+1
            xmsrc(nmulsrc)=xmax
            ymsrc(nmulsrc)=ymax
            pkv(nmulsrc)=maxv
            call clean_sub(im,subn,subm,xmax*1.d0,ymax*1.d0,maxv,bm_pix)
           end if
         endif
        end if
        if (more) goto 333

        return
        end
c! -----------------------------------------------------------------------------------
c! subtracts beam of ampl maxv from integer posn xmax,ymax in im, writes back.
        subroutine clean_sub(im,subn,subm,rx,ry,maxv,bm_pix)
        implicit none
        include "constants.inc"
        integer subn,subm,i,j
        real*8 im(subn,subm),maxv,bm_pix(3),bmaj,bmin,bpa
        real*8 dumr1,dumr2,amp,rx,ry

        do i=1,subn
         do j=1,subm
          bmaj=bm_pix(1)/fwsig
          bmin=bm_pix(2)/fwsig
          bpa=bm_pix(3)/rad
          dumr1=(((i-rx)*dcos(bpa)+(j-ry)*dsin(bpa))/bmaj)
          dumr2=(((j-ry)*dcos(bpa)-(i-rx)*dsin(bpa))/bmin)
          amp=maxv*dexp(-0.5d0*(dumr1*dumr1+dumr2*dumr2))
          im(i,j)=im(i,j)-amp
         end do
        end do

        return
        end
c!
c! ---------------
c!
        subroutine put_iniparm_mulgaus(nmulsrc,max_msrc,max6,a,pkv,xpk,
     /             ypk,bm_pix)
        implicit none
        include "constants.inc"
        integer nmulsrc,max_msrc,max6,xpk(max_msrc),ypk(max_msrc),i
        real*8 a(max6),pkv(max_msrc),bm_pix(3)

        do i=1,nmulsrc
         a(1+6*(i-1))=pkv(i) 
         a(2+6*(i-1))=xpk(i)*1.d0 
         a(3+6*(i-1))=ypk(i)*1.d0 
         a(4+6*(i-1))=bm_pix(1)/fwsig
         a(5+6*(i-1))=a(4+6*(i-1))*0.8d0
         a(6+6*(i-1))=0.d0
        end do

        return
        end

c! write the sourcelist
        subroutine putin_srclist(nn,nnb,ii,nsrc,a,max_msrc,max_msrc6,
     /   nmulsrc,rstd,rav,sstd,sav,chisq,q,delx,dely,ffmt,f1,ctype,crpix
     /   ,cdelt,crval,crota,deconv_s,error,flag,flag_s,e_amp,e_x0,
     /   e_y0,e_maj,e_min,e_pa,e_tot,scratch,bm_pix)
        implicit none
        include "constants.inc"
        integer nn,ii,nsrc,max_msrc6,nmulsrc,i,delx,dely,d,nchar,error
        integer hh,mm,dd,ma,max_msrc,error1,flag(max_msrc),nnb
        real*8 chisq,q,a(max_msrc6),ra,dec,dra,ddec,ss,sa
        real*8 crpix(3),cdelt(3),crval(3),crota(3),rstd,rav
        real*8 sstd(max_msrc),sav(max_msrc),deconv_s(3,max_msrc),d1,d2
        real*8 e_amp(max_msrc),e_x0(max_msrc),e_y0(max_msrc)
        real*8 e_maj(max_msrc),e_min(max_msrc),e_pa(max_msrc)
        real*8 e_tot(max_msrc),tot,bm_pix(3)
        character ffmt*(*),f1*(*),fn*500,s,strcoord*500
        character ctype(3)*8,flag_s(max_msrc)*5,scratch*500

        d1=fwsig*abs(cdelt(1))*3600.d0
        d2=fwsig*abs(cdelt(2))*3600.d0
        fn=scratch(1:nchar(scratch))//f1(1:nchar(f1))//'.header'
        ffmt="(3(i5,1x),4(1Pe11.3,1x),4(0Pf13.9,1x),"//!a42,1x,"//
     /      "16(0Pf10.5,1x),4(1Pe11.3,1x),1Pe9.3,1x,0Pf5.2,1x,i5)"
        do 100 i=1,nmulsrc
         d=6*(i-1)
         if (flag(i).ne.0) then
          call maxit(flag_s(i),d,a,max_msrc6,delx,dely,d1,d2)
         end if
         nsrc=nsrc+1
         call xy2radec(a(2+d)+delx,a(3+d)+dely,ra,dec,error1,ctype,
     /        crpix,cdelt,crval,crota)
         dra=ra*rad
         ddec=dec*rad
         call convertra(dra,hh,mm,ss)
         call convertdec(ddec,s,dd,ma,sa)
         if ('s'.eq.'-') dd=-dd

         tot=a(1+d)*(a(4+d)*a(5+d)*fwsig*fwsig)/
     /       (bm_pix(1)*bm_pix(2))
         if (tot.lt.a(1+d)) tot=a(1+d)
         e_tot(i)=e_amp(i)*sqrt(tot/a(1+d))

c         call system('rm -f '//scratch(1:nchar(scratch))//'a')
c         open(unit=21,file=scratch(1:nchar(scratch))//'a')
c         write (21,777) 
c     /      hh,mm,ss,e_x0(i)*d1/fwsig/15.d0,dd,ma,sa,e_y0(i)*d2/fwsig
c         close(21)
c         open(unit=21,file=scratch(1:nchar(scratch))//'a',status='old')
c         read (21,'(a42)') strcoord
c         close(21)

         if (error1.ne.0) write (*,*) ' ERROR !!!!'
         write (nn,ffmt) nsrc,ii,flag(i),tot,e_tot(i),
     /    a(1+d),e_amp(i),dra,e_x0(i)*d1/fwsig/15.d0,
     /    ddec,e_y0(i)*d2/fwsig,!strcoord(1:42),
     /    a(2+d)+delx,e_x0(i),a(3+d)+dely,e_y0(i),a(4+d)*d1,e_maj(i)*d1
     /    ,a(5+d)*d2,e_min(i)*d2,a(6+d),e_pa(i),deconv_s(1,i)*d1,0.d0,
     /    deconv_s(2,i)*d2,0.d0,deconv_s(3,i),0.d0,sstd(i),sav(i),rstd,
     /    rav,chisq,q,0
         write (nnb) nsrc,ii,flag(i),tot,e_tot(i),a(1+d),e_amp(i),
     /    dra,e_x0(i)*d1/fwsig/15.d0,ddec,e_y0(i)*d2/fwsig,
!         hh,mm,ss,e_x0(i)*d1/fwsig,dd,ma,sa,e_y0(i)*d2/fwsig,
     /    a(2+d)+delx,e_x0(i),a(3+d)+dely,e_y0(i),a(4+d)*d1,e_maj(i)*d1
     /    ,a(5+d)*d2,e_min(i)*d2,a(6+d),e_pa(i),deconv_s(1,i)*d1,0.d0,
     /    deconv_s(2,i)*d2,0.d0,deconv_s(3,i),0.d0,sstd(i),sav(i),rstd,
     /    rav,chisq,q,0 
100     continue
777     format(i2,1x,i2,1x,f5.2,1x,f7.4,1x,i3,1x,i2,1x,f6.3,1x,
     /         f8.5)

        return
        end

        subroutine maj_gt_min(bmaj,bmin,bpa)
        implicit none
        real*8 bmaj,bmin,bpa

        if (bmaj.lt.bmin) then  ! then exhanges and corrects angle
         bpa=bpa+90.d0
         call xchangr8(bmaj,bmin)
        end if

        return
        end
c!
c!
        subroutine angle2pa(bpa)
        implicit none
        real*8 bpa,pa

        pa=bpa-90.d0            ! converts from angle from +ve x to PA
        pa=mod(pa,360.d0)        
        if (pa.lt.0.d0) pa=pa+360.d0   ! mod 360 properly
        if (pa.gt.180.d0) pa=pa-180.d0 ! PA between 0 and 180 only
        bpa=pa

        return
        end
c!
c! this is to max bad solutions so as to not write a '*' in .gaul file. stupid formatting.
c ffmt="(2(i4,1x),i5,1x,1Pe11.3,1x,a25,1x,8(0Pf7.2,1x),4(1Pe11.3,1x),1Pe9.3,1x,0Pf5.2)"
        subroutine maxit(fl_s,d,a,m6,delx,dely,d1,d2)
        implicit none
        integer m6,d,delx,dely
        real*8 a(m6),d1,d2
        character fl_s*5

        if (fl_s.ne.'00000') then
         if (fl_s(1:1).eq.'1'.and.abs(a(1+d)).gt.1.d99) 
     /        a(1+d)=9.9d90 
         if (fl_s(1:1).eq.'2'.and.abs(a(1+d)).lt.1.d99) 
     /        a(1+d)=0.d0 
         if (fl_s(2:2).eq.'1'.and.abs(a(2+d)+delx).gt.9999.90) 
     /        a(2+d)=9999.99-delx
         if (fl_s(2:2).eq.'2'.and.abs(a(2+d)+delx).gt.9999.90) 
     /        a(2+d)=0.d0-delx
         if (fl_s(3:3).eq.'1'.and.abs(a(3+d)+delx).gt.9999.90) 
     /        a(3+d)=9999.99-delx
         if (fl_s(3:3).eq.'2'.and.abs(a(3+d)+dely).gt.9999.90) 
     /        a(3+d)=0.d0-dely
         if (fl_s(4:4).eq.'1'.and.abs(a(4+d)*d1).gt.9999.90) 
     /        a(4+d)=9999.99d0/d1
         if (fl_s(4:4).eq.'2'.and.abs(a(4+d)*d1).gt.9999.90) 
     /        a(4+d)=0.d0 
         if (fl_s(5:5).eq.'1'.and.abs(a(5+d)*d2).gt.9999.90) 
     /        a(5+d)=9999.99d0/d2
         if (fl_s(5:5).eq.'2'.and.abs(a(5+d)*d2).gt.9999.90) 
     /        a(5+d)=0.d0 
        end if

        return
        end


