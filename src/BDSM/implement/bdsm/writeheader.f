c! (re)write rudimentary header info

        subroutine writeheader(f1,n,m,scop,freq,bw,intsec,cdelt)
        implicit none
        integer n,m,nchar
        real*8 freq,bw,intsec,cdelt,keyvalue
        character f1*(*),head*500,scop*7,fn*500,scratch*500
        character fg*500,extn*10,keyword*500,dir*500,comment*500
        
        fg="paradefine"
        extn=""
        keyword="scratch"
        dir="./"
        call get_keyword(fg,extn,keyword,scratch,keyvalue,
     /    comment,"s",dir)

        fn=scratch(1:nchar(scratch))//f1(1:nchar(f1))
        call system('rm -f a')
        write (head,777) n,m,scop,freq,bw,intsec,cdelt
        head="sed -e '1c\\"//head(1:nchar(head))//"' "//
     /       fn(1:nchar(fn))//" > a "
        call system(head)
        head="mv -f a "//fn(1:nchar(fn))
        call system(head)
        
777     format(i4,1x,i4,1x,a7,1x,1Pe12.5,1x,1Pe12.5,1x,
     /         1Pe12.5,1x,0Pf7.3)

        return
        end


