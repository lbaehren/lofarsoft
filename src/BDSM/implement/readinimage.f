c! read in image instead

        subroutine readinimage(ifl,ofl,n,m,scratch)
        implicit none
        integer n,m,i,j,nchar
        character ifl*(*),ofl*(*),scratch*500

        ifl=scratch(1:nchar(scratch))//ifl
        ofl=scratch(1:nchar(scratch))//ofl
        open(unit=21,file=ifl(1:nchar(ifl)),status='old',
     /       form='unformatted')
        open(unit=22,file=ofl(1:nchar(ofl)),status='old',
     /       form='unformatted')
        read (21) n,m
        do 100 i=1,n
         call rw(m)
100     continue
        close(21)
        close(22)

        return
        end
        

        subroutine rw(m)
        implicit none
        integer m
        real*8 duma(m)

        read (21,*) duma 
        write (22,'(128(e14.6,1x))') duma 

        return
        end

