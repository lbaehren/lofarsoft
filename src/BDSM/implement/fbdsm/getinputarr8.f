
        subroutine getinputarr8(nx,x,lx)
        implicit none
        integer nx,err,x,i
        real*8 lx(x)

444     continue
        call system('rm -f a b')
        call getininp 
        call readininp(nx,err)
        if (err.eq.1) then
         write (*,*) '  Enter proper values : '
         goto 444
        end if
        open(unit=21,file='b',status='old')
         do i=1,nx
          read (21,*) lx(i)
         end do
        close(21)

        return
        end

