c! get value at (1,1) specially

        subroutine getpix11(f1,extn,scratch,n,m,i,j,val)
        implicit none
        character scratch*500,f1*500,extn*20,fn*500
        integer n,m,i,j,error,nchar
        real*8 val,keyvalue,dumr(m)

        fn=scratch(1:nchar(scratch))//f1(1:nchar(f1))//
     /     extn(1:nchar(extn))
        open(unit=22,file=fn(1:nchar(fn)),status='old',
     /       form='unformatted')
        read (22) n,m
        read (22) dumr
        close(22)
        val=dumr(1)

        return
        end

