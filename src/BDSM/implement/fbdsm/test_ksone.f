c

        implicit none
        integer seed,i
        real*8 gasdev,a(100000),ks_gaus,d,prob
        external ks_gaus

        write (*,*) ' seed '
        read (*,*) seed

        do i=1,100000
         a(i)=gasdev(seed)
        end do

        call ksone(a,100000,ks_gaus,d,prob)
        write (*,*) d,prob


        end


