
        subroutine callsort3_2(x,n,va,vb)
        implicit none
        integer x,n,iwksp(n),i
        real*8 va(x),vb(x),wksp(n),ra(n),rb(n)

        do i=1,n
         ra(i)=va(i)
         rb(i)=vb(i)
        end do
        call sort3_2(n,ra,rb,wksp,iwksp)
        do i=1,n
         va(i)=ra(i)
         vb(i)=rb(i)
        end do

        return
        end


