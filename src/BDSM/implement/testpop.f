c! i dont know what i am doing

        implicit none
        integer seed,i
        real*8 rand

        call getseed(seed)
        do i=1,50000
         call ran1(seed,rand)
         write (*,*) ((2e5-(2e5-2e3)*rand)/1e4)**(-1.d0/1.38d0)
        end do
        

        end
