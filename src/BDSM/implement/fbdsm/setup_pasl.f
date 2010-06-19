
        subroutine setup_pasl(xl,yl,npairs,var,num,xplot,yplot,id)
        implicit none
        character xl*6,yl*6
        integer npairs,i,id(2),num
        real*8 var(num,npairs)
        real*4 xplot(npairs),yplot(npairs)

cf2py   intent(in) var, id
cf2py   intent(out) xl, yl, xplot, yplot

        xl='BCNST '
        yl='BCNST '
        do i=1,npairs
         xplot(i)=var(id(1),i)
         yplot(i)=var(id(2),i)
        end do 

        if (id(1).eq.1.or.id(1).eq.7.or.
     /     (id(1).ge.22.and.id(1).le.27).or.
     /      id(1).eq.47.or.id(1).eq.48) then
         xl(6:6)='L'
         do i=1,npairs
          xplot(i)=log10(xplot(i))
         end do 
        end if
        if (id(2).eq.1.or.id(2).eq.7.or.id(2).eq.28.or.
     /     (id(2).ge.22.and.id(2).le.27).or.
     /      id(2).eq.47.or.id(2).eq.48) then
         yl(6:6)='L'
         do i=1,npairs
          yplot(i)=log10(yplot(i))
         end do 
        end if

        return
        end


