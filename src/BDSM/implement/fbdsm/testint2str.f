
        integer i,nchar
        character str*10

        do i=1,100,10
         call int2str(i,str)
         write (*,*) '|'//str(1:nchar(str))//'|'
        end do

        end
