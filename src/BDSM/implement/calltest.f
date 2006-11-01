c! 

        subroutine calltest(scratch,seed)
        implicit none
        character opt*4,err*1
        character scratch*500,keyword*500,dir*500,fg*500
        character extn*10,comment*500,srldir*500
        real*8 keyvalue
        integer seed

        call writemenutest

333     continue
        write (*,*)    
        write (*,'(a,$)') '   Test option ---> '
        read (*,*) opt
        call checkopttest(opt,err)
        if (err.eq.'y') goto 333

        fg="paradefine"
        extn=""
        dir="./"
        keyword="scratch"
        call get_keyword(fg,extn,keyword,scratch,keyvalue,
     /    comment,"s",dir)

        if (opt.eq.'moma') call testmoma(scratch,seed)
        if (opt.eq.'quit') call testquit
        
        if (opt.ne.'quit') goto 333

        end
c!
c!
c!     ##################    START  OF  SUBROUTINES    ##################
c!
c!
        subroutine writemenutest
        implicit none

c! options are moma quit
        call system('rm -f a')
        open(unit=21,file='a',status='unknown')
        write (21,*) 
        write (21,*) 
        write (21,*) '  Available tests : '
        write (21,*) 
        write (21,*) '  moma : moment analysis'
        write (21,*) '  quit : quit now'
        write (21,*)   
        close(21)
        call system('xterm -geometry 40x15 -T test -hold -e cat a & ')

        return
        end

        subroutine testmoma(scratch,seed)
        implicit none
        integer err,seed
        character ipfile*500
        character scratch*500
        logical exists

444     continue
        write (*,*) '    filename'
        call system('rm -f a b')
        call getininp 
        call readininp(1,err)
        if (err.eq.1) goto 444
        open(unit=21,file='b',status='old')
         read (21,*) ipfile
        close(21)
        call testmomentanal(ipfile,scratch,seed)
        call system('rm -f a b')

        return
        end



        subroutine checkopttest(opt,err)
        implicit none
        integer nmenu
        parameter (nmenu=2)
        character opt*4,err*1,options(nmenu)*4
        integer i
        
        data options/'moma','quit'/

        err='y'
        do 100 i=1,nmenu
         if (opt.eq.options(i)) err='n'
100     continue
        
        return
        end


        subroutine testquit
        implicit none
        character str1*500

        str1='ps -ef|grep xterm|grep geomet|grep test|'//
     /       'awk Z{print "kill -9 " $2}Z > g ; source g'
        str1(45:45)="'"
        str1(67:67)="'"
        call system(str1)
        write (*,*) 

        return
        end


