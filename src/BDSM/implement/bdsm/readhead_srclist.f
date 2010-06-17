
        subroutine readhead_srclist(nn,nch,str,headstr,headint,
     /             headreal,taip)
        implicit none
        integer nn,nch,headint,nchar
        character str*500,head*500,headstr*500,taip
        real*8 headreal

333     read (nn,*) head
        if (head(1:nch).eq.str(1:nch)) then
         backspace(nn)
         if (taip.eq.'i') read (nn,*) head,headint
         if (taip.eq.'s') read (nn,*) head,headstr
         if (taip.eq.'r') read (nn,*) head,headreal
        else
         goto 333
        end if

        return
        end

