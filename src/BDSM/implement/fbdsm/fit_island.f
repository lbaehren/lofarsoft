c! fits an island with multiple gaussians.
c! writes resultant gaussians to a .gaul file.
c! CHANGE GAUL FORMAT

        subroutine fit_island(subim,subrms,f1,xpix,ypix,npix_isl,maxmem,
     /     blc,trc,av,subn,subm,rstd,rav,max_msrc,max_msrc6,nn,nnb,nnc,
     /     nnd,nne,nnf,nng,ii,nsrc,nsrcini,ffmt,ctype,crpix,cdelt,crval,
     /     crota,bm_pix,delx,
     /     dely,gauperisl,scratch,thresh_pix,avclip,minpix_isl,
     /     wcs,wcslen,blankn,blankv,blankneed,submask,maxsize_beam,
     /     linear,bmaj,bmin,bpa,xmax,ymax,runcode,gaus,shap,maxnmax,
     /     fitfreely,iniguess,flagsmallsrc)
        implicit none
<<<<<<< HEAD
        include "constants.inc"
=======
        include "includes/constants.inc"
>>>>>>> Updating source file list for noise; cleanining up include statements, as we no longer allow for symbolic links inside the source directory.
        integer max_msrc,max_msrc6,nn,ii,nnb,nnc,nmulsrc1,xmax,ymax
        integer n,m,npix_isl,maxmem,blc(2),trc(2),gauperisl,error1,nng
        integer xpix(maxmem),ypix(maxmem),i,j,subn,subm,nnd,nne,nnf
        real*8 pkv(max_msrc),a(max_msrc6),bmsampl,fitfreely
        character f1*(*),hist*10,ffmt*500,linear*500,wcsimp*1
        integer mask(subn,subm),delx,dely,error,wcslen,wcs(wcslen)
        real*8 rmask(subn,subm),bm_pix(3),deconv_s(3,max_msrc)
        real*8 sstd(max_msrc),sav(max_msrc),bm_pix_p(3)
        integer maxnmax
        integer nmulsrc,ia(max_msrc6),nsrc,nmul6,d,submask(subn,subm)
        integer xpk(max_msrc),ypk(max_msrc),flag(max_msrc),nsrcini
        real*8 spin,redchi,espin,resid(subn,subm),minpix_isl
        real*8 subim(subn,subm),av,rstd,rav,thresh_pix,maxsize_beam
        real*8 crpix(3),cdelt(3),crval(3),crota(3),subrms(subn,subm)
        real*8 e_amp(max_msrc),e_x0(max_msrc),e_y0(max_msrc)
        real*8 e_maj(max_msrc),e_min(max_msrc),e_pa(max_msrc)
        real*8 e_d1(max_msrc),e_d2(max_msrc),e_d3(max_msrc)
        real*8 e_tot(max_msrc),keyvalue,avclip,chisq,q,blankn,blankv
        character ctype(3)*8,flag_s(max_msrc),dir*500,fg*500,scratch*500
        character extn*20,keyword*500,keystrng*500,comment*500,runcode*2
        character iniguess*500,calctot*1,flagsmallsrc*500
        logical blankneed,gaus,shap
        real*8 bmaj,bmin,bpa,ang_n
        real*8 dumr1(max_msrc),dumr2(max_msrc),dumr3(max_msrc)
        real*8 dumr4(max_msrc),dumr5(max_msrc),dumr6(max_msrc)

        wcsimp='y'
        calctot='y'
        do i=1,max_msrc6
         ia(i)=1
        end do
c! subtract mean
        do j=1,subm
         do i=1,subn
          subim(i,j)=subim(i,j)-av
         end do
        end do

c! prepare the mask properly to cover the island pixels alone.
        call initialisemask(mask,subn,subm,subn,subm,0)
        do i=1,npix_isl
         mask(xpix(i)-delx,ypix(i)-dely)=1
        end do

c! get correct beam parameters
        if (linear.eq.'true') then
         bm_pix_p(1)=bm_pix(1)
         bm_pix_p(2)=bm_pix(2)
         bm_pix_p(3)=bm_pix(3)
        else
c!       find the local direction of north in a stupid way
         call angle_local_north(xmax,ymax,wcslen,wcs,cdelt,ang_n,3) ! deg anticlockwise from vert for north
         
        end if

c! get number of multiple distinct sources. 
c! get_mulsrc1 finds number of peaks (highest in 8-connective region inside island.
c! get_mulsrc2 'CLEANs' with beam upto some rms to find number of sources to fit.
c! shud choose based on residual rms.
c        call get_mulsrc1(subim,subrms,subn,subm,mask,nmulsrc,
c     /                  xpk,ypk,pkv,max_msrc,f1,thresh_pix,scratch,avclip)


c        call get_mulsrc2(subim,subrms,subn,subm,mask,nmulsrc,
c     /                  xpk,ypk,pkv,max_msrc,f1,bm_pix,thresh_pix)

        if (gaus) then
         call get_mulsrc3(subim,subrms,subn,subm,mask,nmulsrc,xpk,ypk,
     /        pkv,max_msrc,f1,bm_pix,thresh_pix,scratch,avclip,nnc,ii,
     /        nmulsrc1)
c! nmulsrc is max num of gaus and nmulsrc1 is initial guess for fit_mulgauss2

         call put_iniparm_mulgaus(nmulsrc,max_msrc,max_msrc6,a,pkv,xpk,
     /        ypk,bm_pix,nnc)

c! write initial guess to ini.gaul
         call fillin_gaulist(max_msrc,max_msrc6,rstd,rav,sstd,sav,
     /      chisq,q,deconv_s,error,flag,flag_s,e_amp,e_x0,e_y0,e_maj,
     /      e_min,e_pa,e_d1,e_d2,e_d3,e_tot,dumr1,dumr2,dumr3,dumr4,
     /      dumr5,dumr6)

         call putin_srclist(nnd,nne,ii,nsrcini,a,max_msrc,max_msrc6,
     /    nmulsrc,
     /    rstd,rav,sstd,sav,chisq,q,delx,dely,ffmt,f1,ctype,crpix,cdelt,
     /    crval,crota,deconv_s,error,flag,flag_s,e_amp,e_x0,e_y0,
     /    e_maj,e_min,e_pa,e_d1,e_d2,e_d3,e_tot,scratch,bm_pix,.true.,
     /    wcs,wcslen,blc,trc,dumr1,dumr2,dumr3,dumr4,dumr5,dumr6,
     /    wcsimp,calctot)
         
         if (nmulsrc.eq.0) write (*,*) ' ZERO!! ',delx,dely
         nmul6=nmulsrc*6

c! now fit gaussians
c        call fit_mulgauss1(subim,subrms,subn,subm,mask,nmulsrc,
c     /       nmul6,max_msrc,max_msrc6,a,ia,chisq,redchi,q,resid,
c     /       rstd,rav,delx,dely,ii,sstd,sav,cdelt,bm_pix,deconv_s,error
c     /       ,thresh_pix,av,flag,flag_s,e_amp,e_x0,e_y0,e_maj,e_min,
c     /       e_pa,e_d1,e_d2,e_d3,e_tot,minpix_isl,nnc)

         call fit_mulgauss2(subim,subrms,subn,subm,mask,nmulsrc,
     /        nmul6,max_msrc,max_msrc6,a,ia,chisq,redchi,q,resid,
     /        rstd,rav,delx,dely,ii,sstd,sav,cdelt,bm_pix,deconv_s,error
     /        ,thresh_pix,av,flag,flag_s,e_amp,e_x0,e_y0,e_maj,e_min,
     /        e_pa,e_d1,e_d2,e_d3,e_tot,minpix_isl,nnc,nmulsrc1,
     /        wcs,wcslen,blankn,blankv,blankneed,submask,maxsize_beam,
     /        dumr1,dumr2,dumr3,dumr4,dumr5,dumr6,fitfreely,iniguess,
     /        flagsmallsrc)
         if (nmulsrc.gt.gauperisl) gauperisl=nmulsrc
        end if


        if (shap) call fit_shapelets(subim,subrms,subn,subm,mask,
     /       delx,dely,ii,cdelt,bm_pix,thresh_pix,blankn,
     /       blankv,blankneed,submask,runcode,nnf,nng,ffmt,blc,trc,
     /       maxnmax)

        if (gaus) call putin_srclist(nn,nnb,ii,nsrc,a,   
     /   max_msrc,max_msrc6,nmulsrc,rstd,rav,sstd,sav,chisq,q,delx,
     /   dely,ffmt,f1,ctype,crpix,cdelt,
     /   crval,crota,deconv_s,error,flag,flag_s,e_amp,e_x0,e_y0,
     /   e_maj,e_min,e_pa,e_d1,e_d2,e_d3,e_tot,scratch,bm_pix,.true.,
     /   wcs,wcslen,blc,trc,dumr1,dumr2,dumr3,dumr4,dumr5,dumr6,
     /   wcsimp,calctot)

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
        character f1*500,extn*20,keyword*500,keystrng*500
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
        call arrstatmask(im,mask,subn,subm,1,1,subn,subm,stdnew,
     /       avnew,'v')

333     continue
        call maxarraymask(im,mask,subn,subm,xmax,ymax,maxv)
        rx=0.d0
        ry=0.d0
        m1=0.d0
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
        call arrstatmask(im,mask,subn,subm,1,1,subn,subm,stdnew,avnew,
     /       'v')
        if (stdnew.gt.std*1.1d0.and.stdold/stdnew.gt.1.1d0) goto 333

        return
        end
c!
c! ---------------
c! combines mulsrc1 and mulsrc2. first get multiple peaks, then clean, then look at residue.
        subroutine get_mulsrc3(subim,subrms,subn,subm,mask,nmulsrc,
     /             xmsrc,ymsrc,pkv,max_msrc,f1,bm_pix,thresh_pix,
     /             scratch,avclip,nnc,ii,nmulsrc1)
        implicit none
        integer subn,subm,nmulsrc,max_msrc,mask(subn,subm),nnc
        integer xmsrc(max_msrc),ymsrc(max_msrc),i,j,coord,ii,ctr1,j1
        integer xsurr(8),ysurr(8),fac,i1,ndigit,xmax,ymax,nchar,nmulsrc1
        real*8 subim(subn,subm),pkv(max_msrc),im(subn,subm)
        real*8 subrms(subn,subm),thresh_pix,avclip,bm_pix(3)
        real*8 std,stdnew,avnew,maxv,distbm,dumr,keyvalue
        character f1*500,extn*20,keyword*500,keystrng*500,comment*500
        character getchar,f2*500,scratch*500,rc*2,s*10
        logical okay,pixinimage,more
        character ff*500

        call matav(subrms,subn,subm,subn,subm,std)
        call copyarray(subim,subn,subm,subn,subm,1,1,subn,subm,im)

        ndigit=int(log10(max(subn,subm)*1.0))+1      ! number of digits in max(n,m)
        fac=int(10**ndigit)
        nmulsrc=0
        do 100 i=1,subn
         do 110 j=1,subm
          if (subim(i,j)-avclip.ge.thresh_pix*subrms(i,j)
     /        .and.mask(i,j).eq.1) then
           coord=j*fac+i
           call getsurr_8pix(coord,xsurr,ysurr,fac) 
           okay=.true.
           do i1=1,8
            if (pixinimage(xsurr(i1),ysurr(i1),1,subn,1,subm)) then
             if  (mask(xsurr(i1),ysurr(i1)).eq.1) then
c! .ge. since WENSS has adjoining pixels with same flux !!! e.g. WN30000H for pixels 680,28 and 680,29 !!!
              okay=okay.and.(subim(i,j).ge.subim(xsurr(i1),ysurr(i1))) 
             end if
            end if
           end do
           if (okay) then
            ctr1=0              ! if adjoining pixels have same value, dont repeat. 
            do j1=1,nmulsrc     ! shud account for case of more than 2 adjoining pixels of same flux
             if (pkv(j1).eq.subim(i,j).and.(abs(i-xmsrc(j1)).eq.1  ! but thats even more bizarre and 
     /           .or.abs(j-ymsrc(j1)).eq.1)) ctr1=1          ! fit_mulgauss2 shud take care of that.
            end do
            if (ctr1.eq.0) then
             nmulsrc=nmulsrc+1
             xmsrc(nmulsrc)=i
             ymsrc(nmulsrc)=j
             pkv(nmulsrc)=subim(i,j)
             call clean_sub(im,subn,subm,i*1.d0,j*1.d0,subim(i,j),
     /            bm_pix)
            end if
           end if
          end if
110      continue
100     continue
        if (nmulsrc.gt.max_msrc) write (*,*) '   ### ERROR !!!! ',
     /       nmulsrc,max_msrc
        nmulsrc1=nmulsrc   ! take as initial guess in fit_mulgauss2

333     continue                                ! now to check residue
        more=.false.
        call arrstatmask(im,mask,subn,subm,1,1,subn,subm,stdnew,
     /       avnew,'v')
        call maxarraymask(im,mask,subn,subm,xmax,ymax,maxv)


        if (stdnew.gt.std.and.maxv.gt.0.d0) then     ! first cut
         if (maxv-avclip.ge.thresh_pix*std           ! res peak is real
     /       .and.maxv-avclip.ge.2.d0*stdnew) then  ! 2 is a hack
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
          else
          end if
         endif
        end if
        if (more) goto 333
        write (nnc,'(i6,$)') nmulsrc

c! sort anyway in decreasing order of flux in case u screw up later (need for fit_mulgauss2)
        call sort3_1(max_msrc,nmulsrc,pkv,xmsrc,ymsrc)
        
        return
        end
c!
c!
c!  for images where convolving kernel is not the beam, so use moments. 
c!  this is for wavelet transform images.
        subroutine get_mulsrc4(subim,subrms,subn,subm,mask,nmulsrc,
     /             xmsrc,ymsrc,pkv,max_msrc,f1,bm_pix,thresh_pix,
     /             scratch,avclip,nnc,ii,nmulsrc1)
        implicit none
        integer subn,subm,nmulsrc,max_msrc,mask(subn,subm),nnc
        integer xmsrc(max_msrc),ymsrc(max_msrc),i,j,coord,ii,ctr1,j1
        integer xsurr(8),ysurr(8),fac,i1,ndigit,xmax,ymax,nchar,nmulsrc1
        real*8 subim(subn,subm),pkv(max_msrc),im(subn,subm)
        real*8 subrms(subn,subm),thresh_pix,avclip,bm_pix(3)
        real*8 std,stdnew,avnew,maxv,distbm,dumr,keyvalue
        character f1*500,extn*20,keyword*500,keystrng*500,comment*500
        character getchar,f2*500,scratch*500,rc*2,s*10
        logical okay,pixinimage,more
        character ff*500

        call matav(subrms,subn,subm,subn,subm,std)
        call copyarray(subim,subn,subm,subn,subm,1,1,subn,subm,im)

        return
        end

c! -----------------------------------------------------------------------------------
c! subtracts beam of ampl maxv from integer posn xmax,ymax in im, writes back.
c! had got PA convention wrong here. cant believe i didnt find out till
c! now (may 08)
        subroutine clean_sub(im,subn,subm,rx,ry,maxv,bm_pix)
        implicit none
        include "includes/constants.inc"
        integer subn,subm,i,j
        real*8 im(subn,subm),maxv,bm_pix(3),bmaj,bmin,bpa
        real*8 dumr1,dumr2,amp,rx,ry

        do i=1,subn
         do j=1,subm
          bmaj=bm_pix(1)/fwsig
          bmin=bm_pix(2)/fwsig
          bpa=(bm_pix(3)+90.d0)/rad
          dumr1=(((i-rx)*dcos(bpa)+(j-ry)*dsin(bpa))/bmaj)
          dumr2=(((j-ry)*dcos(bpa)-(i-rx)*dsin(bpa))/bmin)
          amp=maxv*dexp(-(0.5d0*(dumr1*dumr1+dumr2*dumr2)))
          im(i,j)=im(i,j)-amp
         end do
        end do

        return
        end
c!
c! ---------------
c!
        subroutine put_iniparm_mulgaus(nmulsrc,max_msrc,max6,a,pkv,xpk,
     /             ypk,bm_pix,nnc)
        implicit none
<<<<<<< HEAD
        include "constants.inc"
=======
        include "includes/constants.inc"
>>>>>>> Updating source file list for noise; cleanining up include statements, as we no longer allow for symbolic links inside the source directory.
        integer nmulsrc,max_msrc,max6,xpk(max_msrc),ypk(max_msrc)
        integer i,nnc
        real*8 a(max6),pkv(max_msrc),bm_pix(3)

        do i=1,nmulsrc
         a(1+6*(i-1))=pkv(i) 
         a(2+6*(i-1))=xpk(i)*1.d0 
         a(3+6*(i-1))=ypk(i)*1.d0 
         a(4+6*(i-1))=bm_pix(1)/fwsig
         a(5+6*(i-1))=0.8d0*a(4+6*(i-1))
         a(6+6*(i-1))=bm_pix(3)
         write (nnc,'(6(e11.4),$)') a(1+6*(i-1)),a(2+6*(i-1)),
     /    a(3+6*(i-1)),a(4+6*(i-1)),a(5+6*(i-1)),a(6+6*(i-1))
        end do

        return
        end
c!
c!
c!
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
