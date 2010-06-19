
        subroutine fitstr8line(xfit,yfit,sig,n,nfit,a,chisq,covar)
        implicit none
        integer n,nfit,ia(2),i
        real*8 xfit(n),yfit(n),sig(n),a(2),chisq,covar(2,2)
        real*8 alpha(2,2),alamda
        data ia/1,1/
        
        a(1)=1.d0
        a(2)=1.d0
        alamda=-1.d0
        do 420 i=1,20
         call mrqmin(xfit,yfit,sig,n,nfit,a,ia,2,covar,alpha,2,chisq,
     /               alamda,2)
420     continue
        alamda=0.d0
        call mrqmin(xfit,yfit,sig,n,nfit,a,ia,2,covar,alpha,2,chisq,
     /              alamda,2)

        return
        end


