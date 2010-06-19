
        implicit none
        integer n,nchar
        character fn*500,a*1

        n=0
        write (*,*) ' enter filename'
        read (*,*) fn
        open(unit=41,file=fn)
200      read (41,*,END=100)  a
         n=n+1
         goto 200
        close(41)
100     continue

        write (*,*) n
        
        end

