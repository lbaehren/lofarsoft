c!

        subroutine read_gsmparadefine(error1,fitsdir,scratch,srldir,
     /             gsmsolnname,plotspectra,runcode,plotdir,spinflux,
     /             plotpairs)
        implicit none
        character fitsdir*500,scratch*500,fitsname*500,srldir*500
        character fg*500,extn*20,keyword*500,comment*500,dir*500
        character gsmsolnname*500,runcode*2,dums*500,spinflux*500
        character plotdir*500,plotspectra*500,plotpairs*500
        real*8 keyvalue
        logical ex
        integer nchar,error,error1

        error1=0
        inquire(file='gsmparadefine',exist=ex)
        if (.not.ex) then
         write (*,*) '  File "gsmparadefine" not found.'
         error1=1
         goto 444
        end if

        fg="gsmparadefine"
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
        keyword='gsmsolnname'
        call get_keyword(fg,extn,keyword,gsmsolnname,keyvalue,
     /       comment,'s',dir,error)
        keyword='plotspectra'
        call get_keyword(fg,extn,keyword,plotspectra,keyvalue,
     /       comment,'s',dir,error)
        keyword='runcode'
        call get_keyword(fg,extn,keyword,dums,keyvalue,
     /       comment,'s',dir,error)
        runcode=dums(1:2)
        keyword='fitsdir'
        call get_keyword(fg,extn,keyword,fitsdir,keyvalue,
     /       comment,'s',dir,error)
        keyword='spinflux'
        call get_keyword(fg,extn,keyword,spinflux,keyvalue,
     /       comment,'s',dir,error)
        keyword='plotpairs'
        call get_keyword(fg,extn,keyword,plotpairs,keyvalue,
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

