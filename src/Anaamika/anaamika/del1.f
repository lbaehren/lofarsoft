
        implicit none
        integer gpi, cisl, dumi, dmax
        parameter (gpi=10)
        integer islnum(gpi), inte, nextint, i
        logical isint

        data islnum/1,2,1,4,2,4,7,8,4,10/
        cisl = 10

        write (*,*) islnum

         call rangeint(islnum,gpi,cisl,dumi,dmax)   
         do inte=1,dmax
          call callisint(islnum,gpi,cisl,inte,isint) 
          nextint=inte
444       continue
          if (.not.isint.and.nextint.lt.cisl) then
           nextint=nextint+1
           call callisint(islnum,gpi,cisl,nextint,isint)
           goto 444
          else
           do i=1,cisl
            if (islnum(i).eq.nextint) islnum(i)=inte
           end do
          end if
         end do
        
        write (*,*) islnum

        end
