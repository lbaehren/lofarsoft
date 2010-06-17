
        implicit none
        integer n,m,x,y,hsize,j,cen
        integer space,ih,jh
        
        write (*,'(a,$)') '  Enter n,m : '
        read (*,*) n,m
        write (*,'(a,$)') '  Central pixel (x,y) : '
        read (*,*) x,y
        write (*,'(a,$)') '  Size of h (3,5) : '
        read (*,*) hsize
        write (*,'(a,$)') '  Enter j : '
        read (*,*) j

        cen=(hsize+1)/2
        space=2**(j-1)
          do ih=1,hsize
           do jh=1,hsize
            write (*,'(i3,1x,i3,a5,$)') 
     /        x+(ih-cen)*space,y+(jh-cen)*space,'     '
           end do
            write (*,*)
          end do


        end



