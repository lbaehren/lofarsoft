c! the fdr routine, at last !

        subroutine callfdr(f1,bmaj,bmin,cdelt,n,m,scratch,
     /             blankn,blankv,vecnm,alpha,pcrit,sigcrit)
        implicit none
        include "constants.inc"
        character f1*500,extn*20,f2*500,keyword*500
        character keystrng*500,comment*500,scratch*500
        integer n,m,i,j,areap,nchar,error,vecnm
        real*8 bmaj,bmin,cdelt(3),normim(n,m),vec(vecnm),alpha,slope
        real*8 pcrit,sum1,blankv,blankn,dumr,dumr1,sigcrit

c!  read image and put the values in a vector
        f2=f1(1:nchar(f1))//'.norm'
        extn='.img'
        call readarray_bin(n,m,normim,n,m,f2,extn)
        if (blankn.gt.0.d0) then 
         call arr8tovec8_bl(normim,n,m,n,m,vec,vecnm,blankv)
        else
         call arr8tovec8(normim,n,m,n,m,vec,vecnm)
        end if

c! calculate p value and then sort them
        do i=1,vecnm
         dumr=0.5d0*erfc(vec(i)/sq2) 
         vec(i)=dumr
        end do
        call sort(n*m,vec)

c! calculate slope
        areap=int(bmaj*bmin/abs(cdelt(1)*cdelt(2))*pi/4.d0/dlog(2.d0))
        sum1=0
        do i=1,areap
         sum1=sum1+1.d0/i
        end do
        slope=alpha/sum1

c! now calc largest intercept and get critical p
        do i=vecnm,1,-1
         if (vec(i).lt.slope*i*1.d0/vecnm) then
          pcrit=vec(i)
          goto 444
         end if
        end do
444     continue

c! need to do inverse erfc. get approx from formula (see dir s_ex/fdr) which is good to
c! 0.0034 (worst, at 2.5 sigma)
        dumr1=1.d0-2.d0*pcrit
        dumr=8.d0/3.d0/pi*(pi-3.d0)/(4.d0-pi)
        sigcrit=sqrt(-2.d0/pi/dumr-dlog(1.d0-dumr1*dumr1)/2.d0+
     /    sqrt((2.d0/pi/dumr+dlog(1.d0-dumr1*dumr1)/2.d0)*
     /    (2.d0/pi/dumr+dlog(1.d0-dumr1*dumr1)/2.d0)-
     /    dlog(1.d0-dumr1*dumr1)/dumr))*sq2
        
        return
        end
