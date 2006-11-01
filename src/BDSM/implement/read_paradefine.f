c!

        subroutine read_paradefine(error,fitsdir,scratch,srldir,
     /             fitsname,solnname,runcode)
        implicit none
        character fitsdir*500,scratch*500,fitsname*500,srldir*500
        character fg*500,extn*10,keyword*500,comment*500,dir*500
        character solnname*500,runcode*2,dums*500
        real*8 keyvalue
        logical ex
        integer nchar,error

        error=0
        inquire(file='paradefine',exist=ex)
        if (.not.ex) then
         write (*,*) '  File "paradefine" not found.'
         error=1
         goto 444
        end if

        fg="paradefine"
        extn=""
        dir="./"
        keyword='srldir'
        call get_keyword(fg,extn,keyword,srldir,keyvalue,
     /       comment,'s',dir)
        keyword='scratch'
        call get_keyword(fg,extn,keyword,scratch,keyvalue,
     /       comment,'s',dir)
        keyword='fitsname'
        call get_keyword(fg,extn,keyword,fitsname,keyvalue,
     /       comment,'s',dir)
        keyword='solnname'
        call get_keyword(fg,extn,keyword,solnname,keyvalue,
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
        if (srldir(nchar(srldir):nchar(srldir)).ne.'/') 
     /      srldir(nchar(srldir)+1:nchar(srldir)+1)='/'

        inquire(file=fitsdir(1:nchar(fitsdir))//
     /          fitsname(1:nchar(fitsname)),exist=ex)
        if (.not.ex) then
         write (*,*) '  File ',fitsdir(1:nchar(fitsdir))//
     /     fitsname(1:nchar(fitsname)),' not found.'
         error=1
        end if
        
444     continue
        return
        end

