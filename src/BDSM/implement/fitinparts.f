c! cn1 and gam1 are const and gamma of each entire stretch of length nbin.
c! cdf is 10^cn (=new cn1) * s^gamma
c! fit each part of source count with power law.

        subroutine fitinparts(sarr,cdfarr,nbin,cn1,gam1) ! break into 500/nbin bits, fit each
        implicit none
        real*8 sarr(500),cdfarr(500),cn1(500),gam1(500)
        real*8 xfit(500),yfit(500),sig(500),alamda,chisq
        real*8 covar(2,2),alpha(2,2),a(2)
        integer nbin,i,j,i1,ia(2)

        data ia/1,1/
        data a/1.0,-1.0/

        do 210 i=1,500-(nbin-1),nbin
         do 220 j=i,i+nbin-1
         i1=j-i+1
         xfit(i1)=dlog10(sarr(j))
         yfit(i1)=dlog10(cdfarr(j))
         sig(i1)=1.d0
220      continue
         alamda=-1.d0
         do 420 j=1,20
          call mrqmin(xfit,yfit,sig,nbin,nbin,a,ia,2,covar,alpha,2,
     /        chisq,alamda,2)
420      continue
         alamda=0.d0
          call mrqmin(xfit,yfit,sig,nbin,nbin,a,ia,2,covar,alpha,2,
     /        chisq,alamda,2)
         cn1((i-1)/nbin+1)=10.d0**a(1)
         gam1((i-1)/nbin+1)=a(2)
210     continue

        return
        end


