
        subroutine calcmedian4(wvec,x,n1,n,med)
        implicit none
        integer n,i,x,n1,n2
        real*4 wvec(x),med,selectnr4,dumr,dvec(n-n1+1),vec(n-n1+1)

        do i=n1,n
         vec(i-n1+1)=wvec(i)
        end do
        n2=n-n1+1

        do i=1,n2
         dvec(i)=vec(i)
        end do

        if (mod(n2,2).eq.1) then
         med=selectnr4((n2+1)/2,n2,dvec)
        else
         med=selectnr4(n2/2,n2,dvec)
         dumr=selectnr4(n2/2+1,n2,dvec)
         med=0.50*(med+dumr)
        end if

        return
        end

