
        subroutine get_numpix_val(f1,extn,n,m,value,code,numpix)
        implicit none
        integer n,m,nchar,numpix,i,j
        real*8 image(n,m),value
        character f1*500,extn*10,code*2

        call readarray_bin(n,m,image,n,m,f1,extn)
        numpix=0
        if (code.ne.'ge'.and.code.ne.'le') then
         write (*,*) '  !!! Unknown eqn !! '
         goto 333
        end if

        if (code.eq.'ge') then
         do i=1,n
          do j=1,m
           if (image(i,j).ge.value) numpix=numpix+1
          end do
         end do
        end if

        if (code.eq.'le') then
         do i=1,n
          do j=1,m
           if (image(i,j).le.value) numpix=numpix+1
          end do
         end do
        end if

333     continue
        
        return
        end

