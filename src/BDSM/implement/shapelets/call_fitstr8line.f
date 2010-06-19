
        subroutine call_fitstr8line(xfit,yfit,mask,sig,n1,n,a,
     /             chisq,covar)
        implicit none
        integer n1,n,mask(n),i,ind
        real*8 xfit(n),yfit(n),sig(n),x(n1),y(n1),s(n1)
        real*8 a(2),covar(2,2),chisq

        ind=0
        do i=1,n
         if (mask(i).eq.1) then
          ind=ind+1
          x(ind)=xfit(i)
          y(ind)=yfit(i)
          s(ind)=sig(i)
         end if
        end do
        if (ind.ne.n1) write (*,*) '  !! SCREAM !! '

        call fitstr8line(x,y,s,n1,n1,a,chisq,covar)

        return
        end


