c! check if keyword is in a file
        
        subroutine check_keyword(f1,extn,keyword,scratch,error)
        implicit none
        integer error,ierr,nchar,i
        real*8 keyvalue
        character f1*500,extn*20,fn*500,dumh*500,keyword*500
        character scratch*500

        fn=scratch(1:nchar(scratch))//f1(1:nchar(f1))//
     /     extn(1:nchar(extn))
        error=1
        open(unit=21,file=fn,status='old')
100     continue
        read (21,*,iostat=ierr) dumh
        if (ierr.lt.0) then
         error=1
         goto 335
        end if
        if (dumh(1:nchar(dumh)).eq.keyword(1:nchar(keyword))) then
         error=0
         goto 335
        end if
        goto 100
335     continue
        close(21)

        return
        end
