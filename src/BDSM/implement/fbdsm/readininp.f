
        subroutine readininp(n,err)
        implicit none
        character str1*500,getchar*1
        integer n,i,err,nargs

        err=0
        call system('wc -w a > b')
        open(unit=21,file='b',status='old')
        read (21,*) nargs
        close(21)
        if (n.ne.nargs) then
         err=1
         goto 333
        end if

        str1="awk '{print $1}' a > b"
        call system(str1)
        do 100 i=2,n
         str1(14:14)=getchar(i)
         str1(21:21)=">"
         call system(str1)
100     continue
333     continue

        return
        end

