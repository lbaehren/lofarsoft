c! takes an image, does shapelet decomp using values of beta, recons, plots noise as fn of beta.

        subroutine varybeta_cart(subim,mask,subn,subm,xcen,ycen,
     /        nmax,beta_init,beta,betamin,betamax,nbin)
        implicit none
        include "../fbdsm/includes/trhermcoef.inc"
        integer subn,subm,mask(subn,subm),nbin,ibeta,minind,nmax
        integer i,j
        real*8 subim(subn,subm),beta,cf(nmax,nmax)
        real*8 beta_init,xcen,ycen,delta,betarr(nbin)
        real*8 rarr(subn,subm),stdres(nbin),avres(nbin),stdmin
        real*8 betamin,betamax,dum(subn,subm)

cf2py   intent(in) subim,mask,xcen,ycen,nmax,beta_init,nhc,hc
cf2py   intent(in) betamin,betamax,nbin
cf2py   intent(out) beta

        call initialiseimage(cf,nmax,nmax,nmax,nmax,0.d0)
        delta=(betamax-betamin)/(nbin*1.d0-1.d0)
        do 400 ibeta=1,nbin
         betarr(ibeta)=betamin+(ibeta-1)*delta
         call decomp_cart_mask(subim,mask,subn,subm,hc,betarr(ibeta),
     /        xcen,ycen,nmax,nhc,cf)
         call recons_cart(subn,subm,hc,betarr(ibeta),xcen,ycen,
     /        nmax,cf,rarr,nhc)
c!       create residual image
         do 300 i=1,subn
          do 310 j=1,subm
           dum(i,j)=subim(i,j)-rarr(i,j)
310       continue
300      continue
c!       get rms in resid
         call arrstatmask(dum,mask,subn,subm,1,1,subn,subm,
     /        stdres(ibeta),avres(ibeta))
400     continue

c! get minimum
        beta=betarr(1)
        stdmin=stdres(1)
        minind=1
        do ibeta=2,nbin
         if (stdres(ibeta).lt.stdmin) then
          stdmin=stdres(ibeta)
          beta=betarr(ibeta)
          minind=ibeta
         end if
        end do
        if (minind.eq.1.or.minind.eq.nbin) beta=beta_init
c        if (minind.eq.1.or.minind.eq.nbin) write (*,*) ' bad beta'
c        if (minind.eq.1.or.minind.eq.nbin) write (*,*) betarr
c        if (minind.eq.1.or.minind.eq.nbin) write (*,*) stdres

        return
        end

