
c! take +/- 5 pts on each side, fit power law, at each S so u have cn and gamma as
c! fns of S. at the end pts, just fit what u have.
        subroutine fitcdf(sarr,cdfarr,cn,gam)
        implicit none
        real*8 sarr(500),cdfarr(500),cn(500),gam(500),sig(11),a(2)
        real*8 xfit(11),yfit(11),alamda,chisq,covar(2,2),alpha(2,2)
        integer i,j,i1,ia(2)

        data ia/1,1/
        data a/1.0d0,1.d0/

        do 100 i=1+5,500-5
         do 110 j=i-5,i+5
           i1=j-(i-5)+1
           xfit(i1)=dlog10(sarr(j))
           yfit(i1)=dlog10(cdfarr(j))
           sig(i1)=1.d0
110      continue

         alamda=-1.d0
         do 120 j=1,20
          call mrqmin(xfit,yfit,sig,11,11,a,ia,2,covar,alpha,2,
     /        chisq,alamda,2)
120      continue
         alamda=0.d0
          call mrqmin(xfit,yfit,sig,11,11,a,ia,2,covar,alpha,2,
     /        chisq,alamda,2)
         cn(i)=a(1)
         gam(i)=a(2)
100     continue

        do 130 i=1,5
         cn(i)=cn(6)
         cn(i+495)=cn(495)
         gam(i)=gam(6)
         gam(i+495)=gam(495)
130     continue

        return
        end
