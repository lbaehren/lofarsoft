c!

        subroutine read_paradefine(error1,fitsdir,scratch,srldir,
     /             fitsname,solnname,runcode,plotdir)
        implicit none
        character fitsdir*500,scratch*500,fitsname*500,srldir*500
        character fg*500,extn*20,keyword*500,comment*500,dir*500
        character solnname*500,runcode*2,dums*500,plotdir*500
        real*8 keyvalue
        logical ex
        integer nchar,error,error1

        error1=0
        inquire(file='paradefine',exist=ex)
        if (.not.ex) then
         write (*,*) '  File "paradefine" not found.'
         error1=1
         goto 444
        end if

        fg="paradefine"
        extn=""
        dir="./"
        keyword='srldir'
        call get_keyword(fg,extn,keyword,srldir,keyvalue,
     /       comment,'s',dir,error)
        keyword='scratch'
        call get_keyword(fg,extn,keyword,scratch,keyvalue,
     /       comment,'s',dir,error)
        keyword='fitsname'
        call get_keyword(fg,extn,keyword,fitsname,keyvalue,
     /       comment,'s',dir,error)
        keyword='solnname'
        call get_keyword(fg,extn,keyword,solnname,keyvalue,
     /       comment,'s',dir,error)
        keyword='runcode'
        call get_keyword(fg,extn,keyword,dums,keyvalue,
     /       comment,'s',dir,error)
        keyword='plotdir'
        call get_keyword(fg,extn,keyword,plotdir,keyvalue,
     /       comment,'s',dir,error)
        runcode=dums(1:2)
        keyword='fitsdir'
        call get_keyword(fg,extn,keyword,fitsdir,keyvalue,
     /       comment,'s',dir,error)

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
         error1=1
        end if
        
444     continue
        return
        end

