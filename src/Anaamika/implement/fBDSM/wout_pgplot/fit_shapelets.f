
        subroutine fit_shapelets(subim,subrms,subn,subm,mask,
     /       delx,dely,ii,cdelt,bm_pix,thresh_pix,blankn,blankv,
     /       blankneed,submask,runcode,nnf,nng,ffmt,blc,trc,maxnmax)
        implicit none
        include "constants.inc"
        include "trhermcoef.inc"
        integer subn,subm,mask(subn,subm),ii,nnf,nng,maxnmax
        integer delx,dely,nmax,submask(subn,subm),blc(2),trc(2)
        real*8 beta,subim(subn,subm),stdav,rsubim(subn,subm)
        real*8 subrms(subn,subm),bm_pix(3),thresh_pix,sresid(subn,subm)
        real*8 cdelt(3),xcen,ycen,stdr,avr
        real*8 blankn,blankv
        character fd*500,runcode*2,ffmt*(*)
        logical blankneed

        if (blankneed.eqv..false.) then
         call matav(subrms,subn,subm,subn,subm,stdav)
        else
         call matavmask(subrms,submask,subn,subm,subn,subm,stdav)
        end if

c! get beta and x_c
        call get_beta_cen_cart(subim,subrms,mask,subn,subm,
     /    beta,xcen,ycen,nmax,nhc,hc,runcode,stdav,ii,bm_pix,delx,dely)
        if (nmax.gt.maxnmax) maxnmax=nmax

        call sub_fit_shapelets(nmax,subim,mask,subn,subm,hc,beta,
     /       xcen,ycen,nhc,ii,nnf,nng,delx,dely,ffmt,blc,trc)

        return
        end
c!
c!
c!
        subroutine sub_fit_shapelets(nmax,subim,mask,subn,subm,hc,beta,
     /       xcen,ycen,nhc,ii,nnf,nng,delx,dely,ffmt,blc,trc)
        implicit none
        integer nmax,subn,subm,nhc,ii,nnf,nng,mask(subn,subm)
        integer delx,dely,i,j,blc(2),trc(2)
        real*8 subim(subn,subm),beta,xcen,ycen,hc(nhc),cf(nmax,nmax)
        real*8 rsubim(subn,subm),sresid(subn,subm),stdr,avr
        real*8 dumr(subn,subm)
        character ffmt*500,f1*500

c! get shapelet coefficients
        call initialiseimage(cf,nmax,nmax,nmax,nmax,0.d0)
        call decomp_cart_mask(subim,mask,subn,subm,hc,beta,xcen,ycen,
     /        nmax,nhc,cf)

c! get reconstructed image
        call recons_cart(subn,subm,hc,beta,xcen,ycen,
     /        nmax,cf,rsubim,nhc)

c! get residual subimage and calc statistics
        do j=1,subm
         do i=1,subn
          sresid(i,j)=subim(i,j)-rsubim(i,j)
          dumr(i,j)=mask(i,j)*rsubim(i,j)
         end do
        end do
        call arrstatmask(sresid,mask,subn,subm,1,1,subn,subm,stdr,
     /       avr,'v')

c! write into shapcoef.c file
        call write_shapfiles(ii,beta,nmax,xcen,ycen,cf,stdr,avr,
     /       nnf,nng,delx,dely,ffmt,blc,trc)

        return
        end



