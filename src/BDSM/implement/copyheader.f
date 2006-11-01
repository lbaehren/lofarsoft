c! (re)write rudimentary header info

        subroutine copyheader(f1,f2,scratch)
        implicit none
        integer n,m,nchar
        real*8 freq,bw,intsec,cdelt
        character f1*(*),f2*(*),head*500,scop*7,fn*500,scratch*500
        
        write (*,*) ' ##### WRONG !! FIX FOR BINARY FILES !!!'
        write (*,*) ' ##### WRONG !! FIX FOR BINARY FILES !!!'
        write (*,*) ' ##### WRONG !! FIX FOR BINARY FILES !!!'

        fn=scratch(1:nchar(scratch))//f1(1:nchar(f1))
        fn=fn(1:nchar(fn))
        open(unit=21,file=fn,status='old',
     /       form='unformatted')
        read (21) n,m,scop,freq,bw,intsec,cdelt
        close(21)
        
        fn=scratch(1:nchar(scratch))//f2(1:nchar(f2))
        fn=fn(1:nchar(fn))
        open(unit=21,file=fn,status='old',
     /       form='unformatted')
        read (21) n,m
        close(21)

        write (head,777) n,m,scop,freq,bw,intsec,cdelt
        head="sed -e '1c\\"//head(1:nchar(head))//"' "//
     /       fn//" > a "
        call system(head)
        head="mv -f a "//fn
        call system(head)
        
777     format(i4,1x,i4,1x,a7,1x,1Pe12.5,1x,1Pe12.5,1x,
     /         1Pe12.5,1x,0Pf7.3)

        return
        end


