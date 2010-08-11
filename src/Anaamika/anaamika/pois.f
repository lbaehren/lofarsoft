
        implicit none
        integer k,tot,fac
        real*8 mean


        write (*,'(a,$)') '  Enter mean, total : '
        read (*,*) mean,tot
        
333     write (*,'(a,$)') '  Enter k (-1 to quit) : '
        read (*,*) k
        if (k.ne.-1) then
         write (*,*) '  Number of occurences = ',
     /    tot*(mean**k)*dexp(-mean)/fac(k)
         goto 333
        end if

        end

        function fac(k)
        implicit none
        integer k,fac,i

        if (k.eq.0) then
         fac=1
        else
         fac=1.d0
         do i=1,k
          fac=fac*i
         end do
        end if

        return
        end
