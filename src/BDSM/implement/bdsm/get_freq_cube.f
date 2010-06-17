c!  do this properly later

        subroutine get_freq_cube(fname,scratch,f,error)
        implicit none
        character fname*500,scratch*500,extn*20,comment*500
        character keystrng*500,keyword*500,dumc*1
        integer error,error1,nchar
        real*8 f,crval3,crpix3,cdelt3,dumr

        error=0
        extn='.header'
        keyword='CTYPE3'
        call get_keyword(fname,extn,keyword,keystrng,dumr,
     /       comment,'s',scratch,error)
        if (keystrng(1:4).eq.'FREQ') then
         dumc='3'
        else
         keyword='CTYPE4'
         call get_keyword(fname,extn,keyword,keystrng,dumr,
     /        comment,'s',scratch,error)
         if (keystrng(1:4).eq.'FREQ') then
          dumc='4'
         else
          error=1
         end if
        end if
        keyword='CRPIX'//dumc
        call get_keyword(fname,extn,keyword,keystrng,crpix3,
     /       comment,'r',scratch,error1)
        keyword='CRVAL'//dumc
        call get_keyword(fname,extn,keyword,keystrng,crval3,
     /       comment,'r',scratch,error1)
        keyword='CDELT'//dumc
        call get_keyword(fname,extn,keyword,keystrng,cdelt3,
     /       comment,'r',scratch,error1)

        f=(1-crpix3)*cdelt3+crval3

        return
        end


