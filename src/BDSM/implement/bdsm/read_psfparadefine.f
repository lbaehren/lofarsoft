
        subroutine read_psfparadefine(generators,nsig,over,kappa2,
     /    snrcut,snrtop,snrbot,snrcutstack,error,error1,gencode,
     /    primarygen,tess_method,tess_sc,tess_fuzzy)
        implicit none
        real*8 keyvalue,nsig,kappa2,dumr,snrcut,snrtop,snrbot,tess_fuzzy
        real*8 snrcutstack
        logical ex
        integer nchar,error,error1,over,round,tess_method
        character fg*500,extn*20,dir*500,keyword*500,comment*500
        character generators*500,keystrng*500,gencode*4,primarygen*500
        character tess_sc*1

cf2py   intent(out) generators,nsig,over,kappa2
cf2py   intent(out) snrcut,snrtop,snrbot,gencode,primarygen,
cf2py   intent(out) tess_method,tess_sc,tess_fuzzy,snrcutstack,error,error1

        error1=0
        inquire(file='psfparadefine',exist=ex)
        if (.not.ex) then
         write (*,*) '  File "psfparadefine" not found.'
         error1=1
         stop
        end if

        fg="psfparadefine"
        extn=""
        dir="./"
        keyword='generators'
        call get_keyword(fg,extn,keyword,generators,keyvalue,
     /       comment,'s',dir,error)
        keyword='kappa-bin'
        call get_keyword(fg,extn,keyword,keystrng,nsig,
     /       comment,'r',dir,error)
        keyword='kappa-all'
        call get_keyword(fg,extn,keyword,keystrng,kappa2,
     /       comment,'r',dir,error)
        keyword='over-ny'
        call get_keyword(fg,extn,keyword,keystrng,dumr,
     /       comment,'r',dir,error)
        over=round(dumr)
        if (over.lt.1) over=2
        keyword='snrcut'
        call get_keyword(fg,extn,keyword,keystrng,snrcut,
     /       comment,'r',dir,error)
        keyword='snrtop'
        call get_keyword(fg,extn,keyword,keystrng,snrtop,
     /       comment,'r',dir,error)
        keyword='snrbot'
        call get_keyword(fg,extn,keyword,keystrng,snrbot,
     /       comment,'r',dir,error)
        keyword='snrcutstack'
        call get_keyword(fg,extn,keyword,keystrng,snrcutstack,
     /       comment,'r',dir,error)
        keyword='gencode'
        call get_keyword(fg,extn,keyword,keystrng,keyvalue,
     /       comment,'s',dir,error)
        gencode=keystrng(1:4)
        keyword='primarygen'
        call get_keyword(fg,extn,keyword,primarygen,keyvalue,
     /       comment,'s',dir,error)
        keyword='tess_sc'
        call get_keyword(fg,extn,keyword,keystrng,keyvalue,
     /       comment,'s',dir,error)
        tess_sc=keystrng(1:1)
        keyword='tess_fuzzy'
        call get_keyword(fg,extn,keyword,keystrng,tess_fuzzy,
     /       comment,'r',dir,error)
        keyword='tess_method'
        call get_keyword(fg,extn,keyword,keystrng,dumr,
     /       comment,'r',dir,error)
        tess_method=round(dumr)

        return
        end

