c! read in av map else fill in constant clip map 

        subroutine get_avmap(f2,n,m,avim)
        implicit none
        integer n,m,nchar,error
        real*8 avim(n,m),keyvalue,avclip,dumr
        character extn*20,keyword*500,keystrng*500,comment*500
        character scratch*500,fg*500,dir*500,mean_map*500,fn*500,f2*500

        fg="paradefine"
        extn=""
        keyword="scratch"
        dir="./"
        call get_keyword(fg,extn,keyword,scratch,keyvalue,
     /    comment,"s",dir,error)

        extn='.bparms'
        keyword='mean_map'
        call get_keyword(f2,extn,keyword,mean_map,keyvalue,
     /       comment,'s',scratch,error)

        if (mean_map.eq.'const') then
         extn='.bstat'
         keyword='mean_clip'
         call get_keyword(f2,extn,keyword,keystrng,avclip,
     /       comment,'r',scratch,error)
         call initialiseimage(avim,n,m,n,m,avclip)
        end if
        if (mean_map.eq.'zero') then
         avclip=0.d0
         call initialiseimage(avim,n,m,n,m,avclip)
        end if
        if (mean_map.eq.'map') then
         fn=f2(1:nchar(f2))//'.mean'
         extn='.img'
         call readarray_bin(n,m,avim,n,m,fn,extn)
        end if

        return
        end


