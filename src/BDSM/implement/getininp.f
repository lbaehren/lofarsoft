
        subroutine getininp
        implicit none
        character str1*70

        write (*,'(a5,$)') '     '
        read (*,'(a70)') str1
        call system('rm -f a')
        open(unit=21,file='a',status='unknown')
        write (21,*) str1
        close(21)

        return
        end

