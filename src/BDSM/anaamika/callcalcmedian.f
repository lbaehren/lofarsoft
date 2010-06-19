
        subroutine callcalcmedian(vec,x,n,med)
        implicit none
        integer x,n,i
        real*8 vec(x),avec(n),med

        do i=1,n
         avec(i)=vec(i)
        end do
        call calcmedian(avec,n,1,n,med)

        return
        end

