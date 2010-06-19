
        subroutine calcmedian(wvec,x,n1,n,med)
        implicit none
        integer n,i,x,n1
        real*8 wvec(x),med
        real*8 vec(n-n1+1)

        do i=n1,n
         vec(i-n1+1)=wvec(i)
        end do
        call c_calcmedian(vec,n-n1+1,med)

        return
        end

        subroutine c_calcmedian(vec,n,med)
        implicit none
        integer n,i
        real*8 med,selectnr,dumr,dvec(n),vec(n)

        do i=1,n
         dvec(i)=vec(i)
        end do

        if (mod(n,2).eq.1) then
         med=selectnr((n+1)/2,n,dvec)
        else
         med=selectnr(n/2,n,dvec)
         dumr=selectnr(n/2+1,n,dvec)
         med=0.5d0*(med+dumr)
        end if

        return
        end

