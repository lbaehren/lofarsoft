c! get optimal value of beta and centre and also order, for cartesian shapelets

        subroutine get_beta_cen_cart(subim,subrms,mask,subn,subm,
     /       beta,xcenn,ycenn,nmax,nhc,hc,runcode,std,ii,bm_pix,
     /       delx,dely)
        implicit none
        integer subn,subm,mask(subn,subm),nmax,nhc,ii,error,i,j,round
        integer delx,dely,nbin
        parameter (nbin=25)
        real*8 subim(subn,subm),subrms(subn,subm),bm_pix(3)
        real*8 beta_init,xcen,ycen,hc(nhc,nhc),std,beta
        real*8 betamin,betamax,xcenn,ycenn,m1,m2(2),m3(3)
        real*8 betarr(nbin),stdres(nbin) ! for python binding
        character sav*1,runcode*2

c! first guess beta. take std from second moment.
        call momentmaskonly(subim,mask,subn,subm,subn,subm,m1,m2,m3)
        beta_init=sqrt(m3(1)*m3(2))*2.d0  ! ad hoc, pretty ok 

c! value for nmax
        nmax=round(sqrt(1.d0*subn*subn+subm*subm)/bm_pix(2))-1
        nmax=min(max(nmax*2+2,10),14)  ! totally ad hoc 

c! value for betamin and betamax
        betamin=1.5d0
        betamax=6.5d0

c! use initial beta to get centre
        sav='y'
        call findcen_cart(subim,mask,subn,subm,beta_init,
     /       nmax,xcen,ycen,sav,runcode)

c! check if it makes sense - inside island. else take centroid if small src.
        call check_cen_shapelet(subim,mask,subn,subm,xcen,ycen,
     /       error,bm_pix,ii)
        if (error.gt.0) call get_centroid_mask(subim,mask,subn,subm,
     /      xcen,ycen)

c! use varybeta to get actual beta
        call varybeta_cart(subim,mask,subn,subm,xcen,ycen,nmax,
     /       beta_init,beta,betamin,betamax,nbin)

c! get centre again with new beta
        sav='n'
        call findcen_cart(subim,mask,subn,subm,beta,
     /       nmax,xcenn,ycenn,sav,runcode)

        call check_cen_shapelet(subim,mask,subn,subm,xcenn,ycenn,
     /       error,bm_pix,ii)
        if (error.gt.0) call get_centroid_mask(subim,mask,subn,subm,
     /      xcenn,ycenn)

        return
        end
c!
c!
c!
        subroutine check_cen_shapelet(subim,mask,subn,subm,
     /             xcen,ycen,error,bm_pix,ii)
        implicit none
        include "constants.inc"
        integer subn,subm,mask(subn,subm),error,round,npix,ii
        real*8 subim(subn,subm),xcen,ycen,bm_pix(3)

        error=0
        if (round(xcen).le.0.or.round(xcen).ge.subn.or.
     /      round(ycen).le.0.or.round(ycen).ge.subm) error=1
        if (error.eq.0) then
         if (mask(round(xcen),round(ycen)).ne.1) error=2
        end if
        
        if (error.gt.0) then
         call intmatsum(mask,subn,subm,subn,subm,npix)
         if (npix/(pi*0.25d0*bm_pix(1)*bm_pix(2)).lt.2.5d0) 
     /       error=error*10    ! expected to fail since source too small
        end if
        if (error.lt.10.and.error.gt.0)  ! centre outside but large src
     /   write (*,*) ii,' shapelet centre bad ',error,xcen,ycen,subn,
     /               subm,npix,(pi*0.25d0*bm_pix(1)*bm_pix(2))
        
        return
        end


