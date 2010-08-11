c! converts coordinates to a name

        subroutine coord2name(ara,dek,filen,nam)
        implicit none
        real*8 ara,dek,ss,sa
        integer hh,mm,dd,ma,nchar,i
        character filen*500,nam*40,s

        call convertra(ara,hh,mm,ss)
        call convertdec(dek,s,dd,ma,sa)
        if (hh.gt.9) then
         write (nam,777) filen(1:nchar(filen)),'.',hh,mm,ss,s,dd,ma,sa
        else
         write (nam,888) filen(1:nchar(filen)),'.',hh,mm,ss,s,dd,ma,sa
        end if
777     format(a,a,i2,i2,f5.2,a1,i2,i2,f5.2)
888     format(a,a,i1,i2,f5.2,a1,i2,i2,f5.2)

        do i=1,nchar(nam)
         if (nam(i:i).eq.' ') nam(i:i)='0'
        end do
        
        return
        end



