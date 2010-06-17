
        subroutine callsort(x,n,vec)
        implicit none
        integer x,n
        real*8 vec(x),avec(n)

        do i=1,n
         avec(i)=vec(i)
        end do
        call sort(n,avec)
        do i=1,n
         vec(i)=avec(i)
        end do

        return
        end


