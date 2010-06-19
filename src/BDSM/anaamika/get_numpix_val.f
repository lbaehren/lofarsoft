
        subroutine get_numpix_val(f1,extn,n,m,value,code,numpix)
        implicit none
        integer n,m,nchar,numpix,i,j
        real*8 image(n,m),value
        character f1*500,extn*20,code*2

        call readarray_bin(n,m,image,n,m,f1,extn)
        numpix=0
        if (code.ne.'ge'.and.code.ne.'le'.and.code.ne.'eq') then
         write (*,*) '  !!! Unknown eqn !! '
         goto 333
        end if

        if (code.eq.'ge') then
         do j=1,m
          do i=1,n
           if (image(i,j).ge.value) numpix=numpix+1
          end do
         end do
        end if

        if (code.eq.'le') then
         do j=1,m
          do i=1,n
           if (image(i,j).le.value) numpix=numpix+1
          end do
         end do
        end if

        if (code.eq.'eq') then
         do j=1,m
          do i=1,n
           if (image(i,j).eq.value) numpix=numpix+1
          end do
         end do
        end if

333     continue
        
        return
        end

