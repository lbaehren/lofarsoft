c! read in rms map else fill in constant rmsclip map 

        subroutine get_rmsmap(f2,n,m,rmsim)
        implicit none
        integer n,m,nchar,error
        real*8 rmsim(n,m),keyvalue,rmsclip
        character extn*20,keyword*500,keystrng*500,comment*500
        character scratch*500,fg*500,dir*500,rmsmap*500,fn*500,f2*500

        fg="paradefine"
        extn=""
        keyword="scratch"
        dir="./"
        call get_keyword(fg,extn,keyword,scratch,keyvalue,
     /    comment,"s",dir,error)

        extn='.bparms'
        keyword='rms_map'
        call get_keyword(f2,extn,keyword,rmsmap,keyvalue,
     /       comment,'s',scratch,error)

        if (rmsmap.eq.'const') then
         extn='.bstat'
         keyword='rms_clip'
         call get_keyword(f2,extn,keyword,keystrng,rmsclip,
     /       comment,'r',scratch,error)
         call initialiseimage(rmsim,n,m,n,m,rmsclip)
        end if

        if (rmsmap.eq.'map') then
         fn=f2(1:nchar(f2))//'.rmsd'
         extn='.img'
         call readarray_bin(n,m,rmsim,n,m,fn,extn)
        end if

        return
        end


