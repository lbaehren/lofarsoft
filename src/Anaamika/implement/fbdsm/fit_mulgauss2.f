c! fits multiple gaussians. modified from ../shapelets/program
c! modify mulgauss1.
c! start from nmulsrc=1 to max=nmulsrc

        subroutine fit_mulgauss2(subim,subrms,subn,subm,mask,
     /      nmulsrc,nmul6,max_msrc,max6,soln,isoln,chisq,redchi,q,
     /      resid,rstd,rav,delx,dely,ii,sstd,sav,cdelt,bm_pix,deconv_s,
     /      error,thresh_pix,avclip,flag,flag_s,e_amp,e_x0,e_y0,
     /      e_maj,e_min,e_pa,e_d1,e_d2,e_d3,e_tot,minpix_isl,nnc,
     /      nmulsrc1,wcs,wcslen,blankn,blankv,blankneed,submask,
     /      maxsize_beam,dumr1,dumr2,dumr3,dumr4,dumr5,dumr6,
     /      fitfreely,iniguess,flagsmallsrc)
        implicit none
        include "constants.inc"
        integer subn,subm,mask(subn,subm),nmulsrc,max_msrc,dof,doff
        integer max6,nmul6,delx,dely,dumi,smask(subn,subm),round,nnc
        integer nmul_o,nmul6_o,ind,dumi1,dumi2,flag(max_msrc),d
        integer i,j,isoln(max6),ia(nmul6),i1,ii,error,quality(nmulsrc)
        integer nmulmax,n6max,nmulsrc1,submask(subn,subm),nchar
        real*8 sgnl(subn,subm),sstd(max_msrc),sav(max_msrc)
        real*8 subim(subn,subm),a(nmul6),stdav,d1,d2,fitfreely
        real*8 bpa,amp,dumr,dumrr1,dumrr2,maxv,avclip,dumrr3
        real*8 sig(subn,subm),gammq,soln(max6),deconv_s(3,max_msrc)
        real*8 subrms(subn,subm),bm_pix(3),thresh_pix,maxsize_beam
        real*8 resid(subn,subm),chisq,redchi,q,chisqold
        real*8 alamda,covar(nmul6,nmul6),cdelt(3),gaus_d(3)
        real*8 alpha(nmul6,nmul6),rstd,rav,gaus_r(3),gaus_o(3)
        real*8 e_amp(max_msrc),e_x0(max_msrc),e_y0(max_msrc)
        real*8 e_maj(max_msrc),e_min(max_msrc),e_pa(max_msrc)
        real*8 e_d1(max_msrc),e_d2(max_msrc),e_d3(max_msrc)
        real*8 e_tot(max_msrc),minpix_isl,blankn,blankv 
        character f1*10,flag_s(max_msrc)*5,fn*500,rc*2
        character fd*500,iniguess*500,flagsmallsrc*500
        real*8 dumrr4,chisqoldold,dumr1(max_msrc),dumr2(max_msrc)
        real*8 dumr3(max_msrc),dumr4(max_msrc),dumr5(max_msrc)
        real*8 dumr6(max_msrc)
        integer wcslen,wcs(wcslen)
        logical blankneed
        integer dumiarr(subm)

        nmulmax=nmulsrc
        n6max=nmul6
        write (nnc,'(i1,$)') 1
        if (blankneed.eqv..false.) then
         call matav(subrms,subn,subm,subn,subm,stdav)
        else
         call matavmask(subrms,submask,subn,subm,subn,subm,stdav)
        end if
         if (stdav.lt.0.d0) write (*,*) ' AIYO ! ',ii,soln(2),
     /       soln(3),stdav
        call initialiseimage(sig,subn,subm,subn,subm,stdav)
        call intmatsum(mask,subn,subm,subn,subm,dof)

c! copy a, ia from soln, isoln (diff sizes) for latest source
        do i=1,nmulsrc*6
         ia(i)=isoln(i)
         a(i)=soln(i)
        end do

c! now start from nmulsrc=1 upto max (initial nmulsrc)
        if (nmulsrc1.eq.0) then
        else
         nmulsrc=1
        end if
        if (iniguess(1:nchar(iniguess)).eq.'all') then
         nmulsrc=nmulmax
        else
         nmulsrc=nmulsrc1
        end if
345     continue

c! fix bmaj, bmin if #pix <= # fitted paras
        if (round(dof*1.d0/nmulsrc).le.6) then
         do i=1,nmulsrc
          a(4+6*(i-1))=bm_pix(1)/fwsig
          a(5+6*(i-1))=bm_pix(2)/fwsig
          a(6+6*(i-1))=bm_pix(3)
          ia(4+6*(i-1))=0
          ia(5+6*(i-1))=0
          if (round(dof*1.d0/nmulsrc).le.4) ia(6+6*(i-1))=0
         end do
        end if
        if (round(dof*1.d0/nmulsrc).lt.4) then
         write (*,*) ' FIX THISS ',ii
        end if

        if (fitfreely.eq.0) then 
         do i=1,nmulsrc
          a(4+6*(i-1))=bm_pix(1)/fwsig
          a(5+6*(i-1))=bm_pix(2)/fwsig
          a(6+6*(i-1))=bm_pix(3)
          ia(4+6*(i-1))=0
          ia(5+6*(i-1))=0
          ia(6+6*(i-1))=0
         end do
        end if

c! a1 is amp in Jy, a2,a3 is centre in pixels, a5, a6 are sigma in pixels
555     continue
        dumi=1
        alamda=-1.d0
        dumi1=nmulsrc*6

334     continue
        call callmrqmin2dm(dumi1,subim,mask,sig,subn,subm,a,ia,nmul6,
     /        covar,alpha,nmul6,chisq,alamda,2)

        if (dumi.eq.1) then
         dumi=dumi+1
         chisqold=chisq
         goto 334
        else
         if (dumi.le.35.or.abs(chisqold-chisq)/chisqold.gt.0.01d0) then
          chisqold=chisq
          dumi=dumi+1
          goto 334
         end if
        end if

        alamda=0.d0
        dumi1=nmulsrc*6
        call callmrqmin2dm(dumi1,subim,mask,sig,subn,subm,a,ia,nmul6,
     /        covar,alpha,nmul6,chisq,alamda,2)

c! sometimes get negative bmaj, bmin, take as good and abs()
        do i=1,nmulsrc
        d=6*(i-1)
         a(4+d)=abs(a(4+d))
         a(5+d)=abs(a(5+d))
        end do

        if (nmulsrc.lt.nmulmax) then
         call residgaus(subim,subn,subm,a,nmul6,nmulsrc*6,resid)
         call maxarraymask(resid,mask,subn,subm,dumi1,dumi2,maxv)

         if (maxv.gt.thresh_pix*stdav) then
          nmulsrc=nmulsrc+1
          d=(nmulsrc-1)*6
          a(d+1)=maxv
          a(d+2)=dumi1*1.d0
          a(d+3)=dumi2*1.d0
          goto 345
          end if
        end if
c! sometimes doesnt converge fast enuff in 20 iter -- eg is op.img -- 
c! so have to check chisq decrement and then stop.

c! bad solutions
        nmul_o=nmulsrc
        nmul6_o=nmul6
        call maxarraymask(subim,mask,subn,subm,dumi1,dumi2,maxv)
        call soln_quality(a,nmulsrc,nmul6,quality,subn,subm,
     /    max_msrc,thresh_pix,stdav,maxv,bm_pix,avclip,flag_s,
     /    wcs,wcslen,delx,dely,maxsize_beam,flagsmallsrc)
        
c! copy a, ia to soln, isoln (diff sizes) 
        do i=1,nmulsrc
         d=6*(i-1)
         dumr=a(6+d)
         call maj_gt_min(a(4+d),a(5+d),a(6+d))  ! corrects if bmaj < bmin
         call angle2pa(a(6+d))                  ! converts to PA 
        end do
        do i=1,nmul6
         soln(i)=a(i)
         isoln(i)=ia(i)
        end do

        doff=dof
        do i=1,nmul6
         if (ia(i).eq.1) doff=doff-1
        end do
        if (round(dof*1.d0/nmulsrc).lt.round(minpix_isl)) then
         do i=1,nmulsrc
          flag(i)=1
         end do
        end if
        
        redchi=chisq/doff
        q=0.d0

c! get residual rms and mean for island
        call initialiseimage(sgnl,subn,subm,subn,subm,0.d0)
        call residgaus(subim,subn,subm,a,nmul6,nmulsrc*6,resid)
        call arrstatmask(resid,mask,subn,subm,1,1,subn,subm,rstd,
     /       rav,'v')

c! gaussian residual rms
        do i1=1,nmulsrc
         flag(i1)=quality(i1)
         if (flag(i1).eq.0) then
          call initialisemask(smask,subn,subm,subn,subm,0)
          do i=1,subn
           do j=1,subm
            d=6*(i1-1)
            bpa=(a(6+d)+90.d0)/rad
            dumrr1=(((i-a(2+d))*dcos(bpa)+(j-a(3+d))*dsin(bpa))/a(4+d))
            dumrr2=(((j-a(3+d))*dcos(bpa)-(i-a(2+d))*dsin(bpa))/a(5+d))
            amp=a(1+d)*dexp(-0.5d0*(dumrr1*dumrr1+dumrr2*dumrr2))
            if (amp.ge.0.21d0*a(1+d)) smask(i,j)=1               ! 1.5 fwhm
           end do
          end do
          call arrstatmask(resid,smask,subn,subm,1,1,subn,subm,
     /        sstd(i1),sav(i1),'v')
         else
          sstd(i1)=9.d9
          sav(i1)=9.d9
         end if
        end do

c! error analysis. do basic stuff from condon for now.
        do i1=1,nmulsrc
         d=6*(i1-1)
         dumr=sqrt(a(4+d)*a(5+d)/(4.d0*bm_pix(1)/fwsig*bm_pix(2)/fwsig))
         dumrr1=1.d0+bm_pix(1)/fwsig*bm_pix(2)/fwsig/(a(4+d)*a(4+d))
         dumrr2=1.d0+bm_pix(1)/fwsig*bm_pix(2)/fwsig/(a(5+d)*a(5+d))
         dumrr3=a(1+d)/stdav
         dumrr3=dumr*dumrr3
         d1=sqrt(8.d0*dlog(2.d0))
         d2=(a(4+d)*a(4+d)-a(5+d)*a(5+d))/(a(4+d)*a(5+d))
         e_amp(i1)=a(1+d)*sq2/(dumrr3*(dumrr1**0.75d0)*(dumrr2**0.75d0))
         e_x0(i1)=a(4+d)*sq2/(d1*dumrr3*(dumrr1**1.25d0)*
     /            (dumrr2**0.25d0))
         e_y0(i1)=a(5+d)*sq2/(d1*dumrr3*(dumrr1**0.25d0)*
     /            (dumrr2**1.25d0))
         e_maj(i1)=a(4+d)*sq2/(dumrr3*(dumrr1**1.25d0)*(dumrr2**0.25d0))
         e_min(i1)=a(5+d)*sq2/(dumrr3*(dumrr1**0.25d0)*(dumrr2**1.25d0))
         e_pa(i1)=2.d0/(d2*dumrr3*(dumrr1**0.25d0)*(dumrr2**1.25d0))
         e_pa(i1)=e_pa(i1)*rad
         e_tot(i1)=a(1+d)*sqrt(e_amp(i1)*e_amp(i1)/(a(1+d)*a(1+d))+
     /      (0.25d0/dumr/dumr)*(e_maj(i1)*e_maj(i1)/(a(4+d)*a(4+d))+
     /      e_min(i1)*e_min(i1)/(a(5+d)*a(5+d))))
         if (abs(e_pa(i1)).gt.180.d0) e_pa(i1)=180.d0 ! stupid hack. wrong !!
        end do

c! deconvolved size
        do i1=1,nmulsrc
         d=6*(i1-1)
         gaus_o(1)=a(4+d)              ! in sig_pix, sig_pix, deg
         gaus_o(2)=a(5+d)
         gaus_o(3)=a(6+d)
         gaus_r(1)=bm_pix(1)/fwsig     ! in sig_pix, sig_pix, deg
         gaus_r(2)=bm_pix(2)/fwsig
         gaus_r(3)=bm_pix(3)
         call deconv(gaus_o,gaus_r,gaus_d,e_maj(i1),e_min(i1),
     /        e_pa(i1),e_d1(i1),e_d2(i1),e_d3(i1),error)
         deconv_s(1,i1)=gaus_d(1)
         deconv_s(2,i1)=gaus_d(2)
         deconv_s(3,i1)=gaus_d(3)
         dumr1(i1)=stdav
         dumr2(i1)=0.d0
         dumr3(i1)=0.d0
         dumr4(i1)=0.d0
         dumr5(i1)=0.d0
         dumr6(i1)=0.d0
        end do


        return
        end


