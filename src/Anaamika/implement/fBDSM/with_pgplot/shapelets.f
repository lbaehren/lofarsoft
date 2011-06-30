        
        subroutine shapelets(scratch,srldir,runcode)
        implicit none
        character opt*4,err*1
        character scratch*500,keyword*500,dir*500,fg*500
        character extn*20,comment*500,srldir*500,runcode*2
        real*8 keyvalue
        integer error

cf2py   intent(in) scratch,srldir,runcode

        call writemenushap

        write (*,*)
        write (*,*) '  You are now inside the shapelets menu'
        write (*,*) '  Quitting will take you back to  noise'
        write (*,*) '                            ---- R.N.M.'
333     continue
        write (*,*)    
        write (*,'(a24,$)') '   Shapelet_Option ---> '
        read (*,*) opt
        call checkoptshap(opt,err)
        if (err.eq.'y') goto 333

        if (opt.eq.'sims') call callsimul_s(scratch,srldir,runcode)
        if (opt.eq.'varb') call callvarybeta(scratch,srldir,runcode)
        if (opt.eq.'shap') call callmainshap(scratch,srldir,runcode)
        if (opt.eq.'quit') call quitshap
        
        if (opt.ne.'quit') goto 333

        end
c!
c!
c!     ##################    START  OF  SUBROUTINES    ##################
c!
c!
        subroutine writemenushap
        implicit none

c! options are quit varb sims shap
        call system('rm -f a')
        open(unit=21,file='a',status='unknown')
        write (21,*) 
        write (21,*) 
        write (21,*) '  Type your favourite 4-letter word '
        write (21,*) 
        write (21,*) '  shap : decompose image into shapelets'
        write (21,*) '  varb : find optimal beta value'
        write (21,*) '  sims : decompose simulated non-gaussian images'
        write (21,*) '  quit : quit'
        write (21,*)   
        close(21)
        call system('xterm -geometry 50x25 -T shap -hold -e cat a & ')

        return
        end


        subroutine callvarybeta(scratch,srldir,runcode)
        implicit none
        integer err
        character ipfile*500,srldir*500,extn*20
        integer n,m
        character scratch*500,runcode*2
        logical exists

444     continue
        write (*,*) '    imagefilename'
        call system('rm -f a b')
        call getininp 
        call readininp(1,err)
        if (err.eq.1) goto 444
        open(unit=21,file='b',status='old')
         read (21,*) ipfile
        close(21)
        extn='.img'
        if (.not.exists(ipfile,scratch,extn)) goto 444
        call varybeta(ipfile,scratch,srldir,runcode)
        call system('rm -f a b')

        return
        end

        subroutine callsimul_s(scratch,srldir,runcode)
        implicit none
        character srldir*500,runcode*2,scratch*500

        call simul_shapelets(scratch,srldir,runcode)

        return
        end

        subroutine callmainshap(scratch,srldir,runcode)
        implicit none
        integer err
        character ipfile*500,runcode*2,f2*500,srldir*500
        integer n,m,nchar
        character scratch*500,solnname*500,extn*20
        logical exists

444     continue
        write (*,*) '    imagefilename '
        call system('rm -f a b')
        call getininp 
        call readininp(1,err)
        if (err.eq.1) goto 444
        open(unit=21,file='b',status='old')
         read (21,*) ipfile
        close(21)
        extn='.img'
        if (.not.exists(ipfile,scratch,extn)) goto 444
        call main_shapelets(ipfile,scratch,srldir,runcode)
        call system('rm -f a b')

        return
        end

        subroutine checkoptshap(opt,err)
        implicit none
        integer nmenu
        parameter (nmenu=4)
        character opt*4,err*1,options(nmenu)*4
        integer i
        
        data options/'quit','sims','varb','shap'/

        err='y'
        do 100 i=1,nmenu
         if (opt.eq.options(i)) err='n'
100     continue
        
        return
        end


        subroutine quitshap
        implicit none
        character str1*500

        str1='ps -ax|grep xterm|grep geomet|grep shap|'//
     /       'awk Z{print "kill -9 " $2}Z > g ; source g'
        str1(45:45)="'"
        str1(67:67)="'"
        call system(str1)
        write (*,*) 
        write (*,*) '  Hope you enjoyed your shapelet session'
        write (*,*) '  Going back to "noise" now    ..... '

        return
        end

        

