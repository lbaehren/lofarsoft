c!

        subroutine read_aiparadefine(error1,fitsdir,scratch,srldir,
     /     fitsname,solnname,airuncode,plotdir,gausshap,saveplots)
        implicit none
        character fitsdir*500,scratch*500,fitsname*500,srldir*500
        character fg*500,extn*20,keyword*500,comment*500,dir*500
        character solnname*500,airuncode*2,dums*500,gausshap*500
        character plotdir*500,saveplots*1
        real*8 keyvalue
        logical ex
        integer nchar,error,error1

        error1=0
        inquire(file='aiparadefine',exist=ex)
        if (.not.ex) then
         write (*,*) '  File "aiparadefine" not found.'
         error1=1
         goto 444
        end if

        fg="aiparadefine"
        extn=""
        dir="./"
        keyword='srldir'
        call get_keyword(fg,extn,keyword,srldir,keyvalue,
     /       comment,'s',dir,error)
        keyword='plotdir'
        call get_keyword(fg,extn,keyword,plotdir,keyvalue,
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
        keyword='airuncode'
        call get_keyword(fg,extn,keyword,dums,keyvalue,
     /       comment,'s',dir,error)
        airuncode=dums(1:2)
        keyword='saveplots'
        call get_keyword(fg,extn,keyword,dums,keyvalue,
     /       comment,'s',dir,error)
        saveplots=dums(1:1)
        keyword='fitsdir'
        call get_keyword(fg,extn,keyword,fitsdir,keyvalue,
     /       comment,'s',dir,error)
        keyword='gausshap'
        call get_keyword(fg,extn,keyword,gausshap,keyvalue,
     /       comment,'s',dir,error)

        if (fitsdir(nchar(fitsdir):nchar(fitsdir)).ne.'/') 
     /      fitsdir(nchar(fitsdir)+1:nchar(fitsdir)+1)='/'
        if (scratch(nchar(scratch):nchar(scratch)).ne.'/') 
     /      scratch(nchar(scratch)+1:nchar(scratch)+1)='/'
        if (srldir(nchar(srldir):nchar(srldir)).ne.'/') 
     /      srldir(nchar(srldir)+1:nchar(srldir)+1)='/'

444     continue
        return
        end

