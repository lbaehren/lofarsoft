c! fits multiple gaussians. modified from ../shapelets/program

        subroutine fit_mulgauss(subim,subrms,subn,subm,mask,
     /      nmulsrc,nmul6,max_msrc,max6,soln,isoln,chisq,redchi,q,
     /      resid,rstd,rav,delx,dely,ii,sstd,sav,cdelt,bm_pix,deconv_s,
     /      error,thresh_pix,avclip,flag,flag_s,e_amp,e_x0,e_y0,
     /      e_maj,e_min,e_pa,e_tot)
        implicit none
        include "constants.inc"
        integer subn,subm,mask(subn,subm),nmulsrc,max_msrc,dof
        integer max6,nmul6,delx,dely,dumi,smask(subn,subm)
        integer nmul_o,nmul6_o,ind,dumi1,dumi2,flag(max_msrc),d
        integer i,j,isoln(max6),ia(nmul6),i1,ii,error,quality(nmulsrc)
        real*8 sgnl(subn,subm),sstd(max_msrc),sav(max_msrc)
        real*8 subim(subn,subm),a(nmul6),stdav,d1,d2
        real*8 bpa,amp,dumr,dumr1,dumr2,maxv,avclip,dumr3
        real*8 sig(subn,subm),gammq,soln(max6),deconv_s(3,max_msrc)
        real*8 subrms(subn,subm),bm_pix(3),thresh_pix
        real*8 resid(subn,subm),chisq,redchi,q,chisqold
        real*8 alamda,covar(nmul6,nmul6),cdelt(3),gaus_d(3)
        real*8 alpha(nmul6,nmul6),rstd,rav,gaus_r(3),gaus_o(3)
        real*8 e_amp(max_msrc),e_x0(max_msrc),e_y0(max_msrc)
        real*8 e_maj(max_msrc),e_min(max_msrc),e_pa(max_msrc)
        real*8 e_tot(max_msrc)
        character f1*10,flag_s(max_msrc)*5

c! copy a, ia from soln, isoln (diff sizes) 
        do i=1,nmul6
         ia(i)=isoln(i)
         a(i)=soln(i)
        end do

        call matav(subrms,subn,subm,subn,subm,stdav)
        call initialiseimage(sig,subn,subm,subn,subm,stdav)
        call intmatav(mask,subn,subm,subn,subm,dof)
        dof=dof*subn*subm
        
c! a1 is amp in Jy, a2,a3 is centre in pixels, a5, a6 are sigma in pixels
555     continue
        dumi=1
        alamda=-1.d0
        call mrqmin2dm(subim,mask,sig,subn,subm,a,ia,nmul6,   ! fit in pixels
     /       covar,alpha,nmul6,chisq,alamda,2)
        chisqold=chisq

334     call mrqmin2dm(subim,mask,sig,subn,subm,a,ia,nmul6,
     /        covar,alpha,nmul6,chisq,alamda,2)
        if (dumi.le.35.or.abs(chisqold-chisq)/chisqold.gt.0.01d0) then
         chisqold=chisq
         dumi=dumi+1
         goto 334
        end if

        alamda=0.d0
        call mrqmin2dm(subim,mask,sig,subn,subm,a,ia,nmul6,
     /       covar,alpha,nmul6,chisq,alamda,2)
c! sometimes doesnt converge fast enuff in 20 iter -- eg is op.img -- 
c! so have to check chisq decrement and then stop.

c! bad solutions
        nmul_o=nmulsrc
        nmul6_o=nmul6
        call maxarraymask(subim,mask,subn,subm,dumi1,dumi2,maxv)
        call soln_quality(a,nmulsrc,nmul6,quality,subn,subm,
     /             max_msrc,thresh_pix,stdav,maxv,bm_pix,avclip,flag_s)
        if (nmulsrc.gt.1) then
         ind=1
444      if (quality(ind).ne.0) then  ! source ind is bad
           call pushbacksrc(ind,nmul_o,nmul6_o,nmulsrc,nmul6,a,ia
     /         ,quality)  ! delete source
         end if
         if (ind.lt.nmulsrc) then
          ind=ind+1
          goto 444
         end if
        end if
        do i=1,nmulsrc
         flag(i)=quality(i)
        end do
c!
c! if bad solutions, go back and redo with one less gaussian
        if (nmul_o.ne.nmulsrc.and.nmulsrc.gt.1) then
         goto 555 
        end if
        
c! copy a, ia to soln, isoln (diff sizes) 
        do i=1,nmulsrc
         d=6*(i-1)
         dumr=a(6+d)
         call maj_gt_min(a(4+d),a(5+d),a(6+d))  ! corrects if bmaj < bmin
         call angle2pa(a(6+d))      ! converts to PA 
        end do
        do i=1,nmul6
         soln(i)=a(i)
        end do

        do 260 i=1,nmul6
         if (ia(i).eq.1) dof=dof-1
260     continue
        redchi=chisq/dof
c        write (*,*) 'Chisq = ',chisq,'; dof = ',dof,'; redchi = ',redchi
cc        q=gammq(dof/2.d0,chisq/2.d0)
cc        write (*,*) 'Q parameter is ',q
c        if (redchi.gt.1.5d0) write (*,*) 'Did not converge',redchi
c     /            ,chisq,dof
        q=0.d0

c! get residual rms and mean for island
        call initialiseimage(sgnl,subn,subm,subn,subm,0.d0)
        do i=1,subn
         do j=1,subm
          do i1=1,nmulsrc
           d=6*(i1-1)
           bpa=(a(6+d)+90.d0)/rad
           dumr1=(((i-a(2+d))*dcos(bpa)+(j-a(3+d))*dsin(bpa))/a(4+d))
           dumr2=(((j-a(3+d))*dcos(bpa)-(i-a(2+d))*dsin(bpa))/a(5+d))
           amp=a(1+d)*dexp(-0.5d0*(dumr1*dumr1+dumr2*dumr2))
           sgnl(i,j)=sgnl(i,j)+amp
          end do
          resid(i,j)=subim(i,j)-sgnl(i,j)
         end do
        end do
        call arrstatmask(resid,mask,subn,subm,1,1,subn,subm,rstd,rav)

c! source residual rms
        do i1=1,nmulsrc
         call initialisemask(smask,subn,subm,subn,subm,0)
         do i=1,subn
          do j=1,subm
           d=6*(i1-1)
           bpa=(a(6+d)+90.d0)/rad
           dumr1=(((i-a(2+d))*dcos(bpa)+(j-a(3+d))*dsin(bpa))/a(4+d))
           dumr2=(((j-a(3+d))*dcos(bpa)-(i-a(2+d))*dsin(bpa))/a(5+d))
           amp=a(1+d)*dexp(-0.5d0*(dumr1*dumr1+dumr2*dumr2))
           if (amp.ge.0.21d0*a(1+d)) smask(i,j)=1               ! 1.5 fwhm
          end do
         end do
         call arrstatmask(resid,smask,subn,subm,1,1,subn,subm,
     /        sstd(i1),sav(i1))
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
         call deconv(gaus_o,gaus_r,gaus_d,error)
         deconv_s(1,i1)=gaus_d(1)
         deconv_s(2,i1)=gaus_d(2)
         deconv_s(3,i1)=gaus_d(3)
        end do
c
c! error analysis. do basic stuff from condon for now.
        do i1=1,nmulsrc
         d=6*(i1-1)
         dumr=sqrt(a(4+d)*a(5+d)/(4.d0*bm_pix(1)/fwsig*bm_pix(2)/fwsig))
         dumr1=1.d0+bm_pix(1)/fwsig*bm_pix(2)/fwsig/(a(4+d)*a(4+d))
         dumr2=1.d0+bm_pix(1)/fwsig*bm_pix(2)/fwsig/(a(5+d)*a(5+d))
         dumr3=a(1+d)/stdav
         dumr3=dumr*dumr3
         d1=sqrt(8.d0*dlog(2.d0))
         d2=(a(4+d)*a(4+d)-a(5+d)*a(5+d))/(a(4+d)*a(5+d))
         e_amp(i1)=a(1+d)*sq2/(dumr3*(dumr1**0.75d0)*(dumr2**0.75d0))
         e_x0(i1)=a(4+d)*sq2/(d1*dumr3*(dumr1**1.25d0)*(dumr2**0.25d0))
         e_y0(i1)=a(5+d)*sq2/(d1*dumr3*(dumr1**0.25d0)*(dumr2**1.25d0))
         e_maj(i1)=a(4+d)*sq2/(dumr3*(dumr1**1.25d0)*(dumr2**0.25d0))
         e_min(i1)=a(5+d)*sq2/(dumr3*(dumr1**0.25d0)*(dumr2**1.25d0))
         e_pa(i1)=2.d0/(d2*dumr3*(dumr1**0.25d0)*(dumr2**1.25d0))
         e_tot(i1)=a(1+d)*sqrt(e_amp(i1)*e_amp(i1)/(a(1+d)*a(1+d))+
     /      (0.25d0/dumr/dumr)*(e_maj(i1)*e_maj(i1)/(a(4+d)*a(4+d))+
     /      e_min(i1)*e_min(i1)/(a(5+d)*a(5+d))))
        end do
        

        return
        end


        subroutine pushbacksrc(ind,nmul_o,nmul6_o,nmul,nmul6,a,ia,
     /             quality)
        implicit none
        integer ind,nmul,nmul6,nmul_o,nmul6_o,ia(nmul6_o)
        integer quality(nmul_o),i,j,d
        real*8 a(nmul6_o)

        do i=ind,nmul_o-1
         quality(i)=quality(i+1)
         d=6*(i-1)
         do j=1,6
          a(j+d)=a(j+6*i)
          ia(j+d)=ia(j+6*i)
         end do
        end do
        nmul=nmul-1
        nmul6=nmul6-6

        return
        end



