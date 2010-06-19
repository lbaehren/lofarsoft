
        subroutine getnlines_pasl(scrat,scratch,nlines)
        implicit none
        character scrat*500,fn*500,extn*20,scratch*500,cmd*500
        integer nchar,nlines

        extn='.pasf'
        fn=scratch(1:nchar(scratch))//scrat(1:nchar(scrat))//
     /     extn(1:nchar(extn))
        call system('rm -f '//scratch(1:nchar(scratch))//'aa')
        cmd='wc -l '//fn(1:nchar(fn))//' > '//
     /       scratch(1:nchar(scratch))//'aa'
        call system(cmd)
        open(unit=31,file=scratch(1:nchar(scratch))//'aa',status='old')
        read(31,*) nlines
        close(31)
        nlines=nlines-1 ! 1st line is freqs
        call system('rm -f '//scratch(1:nchar(scratch))//'aa')

        return
        end
