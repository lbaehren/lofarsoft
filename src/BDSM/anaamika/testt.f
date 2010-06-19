
        implicit none
        integer i,n,error
        character f1*500,extn*20,keyword*500,keystrng*500
        character comment*500,code*1,scratch*500
        real*8 keyvalue

        scratch='/data/niruj_pers/image/'
        f1='subim'
        extn='.bstat'
        code='r'
        keyword='rms_clip'
        call get_keyword(f1,extn,keyword,keystrng,keyvalue,
     /             comment,code,scratch,error)
        write (*,*) keyvalue

        extn='.bparms'
        keyword='boxsize_th'
        call get_keyword(f1,extn,keyword,keystrng,keyvalue,
     /             comment,code,scratch,error)
        write (*,*) keyvalue

        keyword='colour'
        call get_keyword(f1,extn,keyword,keystrng,keyvalue,
     /             comment,code,scratch,error)
        write (*,*) keyvalue

        keyword='omega'
        call get_keyword(f1,extn,keyword,keystrng,keyvalue,
     /             comment,code,scratch,error)
        write (*,*) keyvalue

        
        end

