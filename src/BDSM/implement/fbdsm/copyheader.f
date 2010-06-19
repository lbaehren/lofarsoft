c! (re)write rudimentary header info

        subroutine copyheader(extn,f1,f2,scratch)
        implicit none
        integer n,m,nchar,n1,m1,l1
        real*8 freq,bw,intsec,cdelt
        character f1*500,f2*500,scop*7
        character fn*500,scratch*500,extn*20

cf2py   intent(in) f1,f2,scratch,extn
        
        fn=scratch(1:nchar(scratch))//f1(1:nchar(f1))//
     /     extn(1:nchar(extn))
        open(unit=21,file=fn(1:nchar(fn)),status='old',
     /       form='unformatted')
        read (21) n,m,scop,freq,bw,intsec,cdelt
        close(21)
        
        call readarraysize(f2,extn,n1,m1,l1)
        if (l1.gt.1) write (*,*) '  Using 2d array for 3d image !!!'
        call sub_puthead(f2,scratch,extn,n1,m1,scop,freq,bw,
     /       intsec,cdelt)

        return
        end


