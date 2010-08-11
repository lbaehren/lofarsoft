c! does what remag in aips does

        subroutine callremag(f1,f2,scratch,runcode)
        implicit none
        character f1*500,f2*500,scratch*500,extn*20,runcode*2
        integer n,m,l
        real*8 blankv,newval,tol

cf2py   intent(in) f1,f2,scratch,runcode

        extn='.img'
        call readarraysize(f1,extn,n,m,l)
        if (l.gt.1) write (*,*) '  Using 2d array for 3d image !!!'
        write (*,'(a,$)') '   What is the old blank value ? '
        read (*,*) blankv
        write (*,'(a,$)') '   Enter the new blank value : '
        read (*,*) newval
        write (*,'(a,$)') '   Whats the tolerance % if any ? '
        read (*,*) tol
        if (blankv.eq.newval) then
         write (*,*) '  Dont be silly'
         goto 334
        else
         call remag(f1,f2,scratch,n,m,blankv,newval,runcode,tol)
        end if
334     continue

        return
        end
c!
c!
c!
        subroutine remag(f1,f2,scratch,n,m,blankv,newval,runcode,tol)
        implicit none
        character f1*500,f2*500,scratch*500,extn*20,runcode*2
        integer n,m,i,j
        real*8 blankv,newval,image(n,m),tol

        extn='.img'
        call readarray_bin(n,m,image,n,m,f1,extn)
        
        if (tol.eq.0.d0) then
         do j=1,m
          do i=1,n
           if (image(i,j).eq.blankv) image(i,j)=newval
          end do
         end do
        else
         if (blankv.ne.0.d0) then
          do j=1,m
           do i=1,n
            if (abs((blankv-image(i,j))/blankv)*100.d0.le.tol)
     /       image(i,j)=newval
           end do
          end do
         else
          do j=1,m
           do i=1,n
            if (abs(blankv-image(i,j)).le.tol)
     /       image(i,j)=newval
           end do
          end do
         end if   ! if blank != 0
        end if

        call writearray_bin2D(image,n,m,n,m,f2,runcode)

        return
        end



