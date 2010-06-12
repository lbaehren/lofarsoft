c! read in rms map else fill in constant rmsclip map 

        subroutine get_rmsmap(f1,n,m,rmsim)
        implicit none
        integer n,m,nchar
        real*8 rmsim(n,m),keyvalue,rmsclip
        character f1*500,extn*10,keyword*500,keystrng*500,comment*500
        character scratch*500,fg*500,dir*500,rmsmap*500,fn*500

        fg="paradefine"
        extn=""
        keyword="scratch"
        dir="./"
        call get_keyword(fg,extn,keyword,scratch,keyvalue,
     /    comment,"s",dir)

        extn='.bstat'
        keyword='rms_map'
        call get_keyword(f1,extn,keyword,rmsmap,keyvalue,
     /       comment,'s',scratch)

        if (rmsmap.eq.'const') then
         keyword='rms_clip'
         call get_keyword(f1,extn,keyword,keystrng,rmsclip,
     /       comment,'r',scratch)
         call initialiseimage(rmsim,n,m,n,m,rmsclip)
        end if

        if (rmsmap.eq.'map') then
         fn=f1(1:nchar(f1))//'.rmsd'
         extn='.img'
         call readarray_bin(n,m,rmsim,n,m,fn,extn)
        end if

        return
        end


