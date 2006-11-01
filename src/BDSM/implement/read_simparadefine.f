c! Reads paradefine file for simulation.f

        subroutine read_simparadefine(fitsdir,scratch,runcode)
        implicit none
        character fitsdir*500,scratch*500
        character fg*500,extn*10,keyword*500,comment*500,dir*500
        character runcode*2,dums*500
        real*8 keyvalue
        logical ex
        integer nchar

        inquire(file='paradefine',exist=ex)
        if (.not.ex) then
         write (*,*) '  File "paradefine" not found.'
         goto 444
        end if

        fg="paradefine"
        extn=""
        dir="./"
        keyword='scratch'
        call get_keyword(fg,extn,keyword,scratch,keyvalue,
     /       comment,'s',dir)
        keyword='runcode'
        call get_keyword(fg,extn,keyword,dums,keyvalue,
     /       comment,'s',dir)
        runcode=dums(1:2)
        keyword='fitsdir'
        call get_keyword(fg,extn,keyword,fitsdir,keyvalue,
     /       comment,'s',dir)

        if (fitsdir(nchar(fitsdir):nchar(fitsdir)).ne.'/') 
     /      fitsdir(nchar(fitsdir)+1:nchar(fitsdir)+1)='/'
        if (scratch(nchar(scratch):nchar(scratch)).ne.'/') 
     /      scratch(nchar(scratch)+1:nchar(scratch)+1)='/'

444     continue
        return
        end

