c! 

        subroutine calltest(seed,scratch,srldir)
        implicit none
        character opt*4,err*1
        character scratch*500,keyword*500,dir*500,fg*500
        character extn*20,comment*500,srldir*500
        real*8 keyvalue
        integer seed,error

cf2py   intent(in) scratch,srldir
cf2py   intent(in,out) seed

        call writemenutest

333     continue
        write (*,*)    
        write (*,'(a,$)') '   Test option ---> '
        read (*,*) opt
        call checkopttest(opt,err)
        if (err.eq.'y') goto 333

        if (opt.eq.'moma') call testmoma(scratch,seed)
        if (opt.eq.'psf1') call testpsf1(scratch,srldir)
        if (opt.eq.'psf2') call testpsf2(scratch,srldir)
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

c! options are moma quit psf1 psf2
        call system('rm -f a')
        open(unit=21,file='a',status='unknown')
        write (21,*) 
        write (21,*) 
        write (21,*) '  Available tests : '
        write (21,*) 
        write (21,*) '  moma : moment analysis'
        write (21,*) '  psf1 : analyse psf 1'
        write (21,*) '  psf2 : analyse psf 2'
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

        subroutine testpsf1(scratch,srldir)
        implicit none
        integer err,seed
        character ipfile*500,scratch*500,srldir*500
        logical exists

444     continue
        write (*,*) '    gaulname'
        call system('rm -f a b')
        call getininp 
        call readininp(1,err)
        if (err.eq.1) goto 444
        open(unit=21,file='b',status='old')
         read (21,*) ipfile
        close(21)
        call analysepsf1(ipfile,scratch,srldir)
        call system('rm -f a b')

        return
        end

        subroutine testpsf2(scratch,srldir)
        implicit none
        integer err,seed
        character ipfile*500,scratch*500,srldir*500
        logical exists

444     continue
        write (*,*) '    gaulname'
        call system('rm -f a b')
        call getininp 
        call readininp(1,err)
        if (err.eq.1) goto 444
        open(unit=21,file='b',status='old')
         read (21,*) ipfile
        close(21)
        call analysepsf2(ipfile,scratch,srldir)
        call system('rm -f a b')

        return
        end


        subroutine checkopttest(opt,err)
        implicit none
        integer nmenu
        parameter (nmenu=4)
        character opt*4,err*1,options(nmenu)*4
        integer i

        data options/'moma','quit','psf1','psf2'/

        err='y'
        do 100 i=1,nmenu
         if (opt.eq.options(i)) err='n'
100     continue
        
        return
        end


        subroutine testquit
        implicit none
        character str1*500

        str1='ps -ax|grep xterm|grep geomet|grep test|'//
     /       'awk Z{print "kill -9 " $2}Z > g ; source g'
        str1(45:45)="'"
        str1(67:67)="'"
        call system(str1)
        write (*,*) 

        return
        end


