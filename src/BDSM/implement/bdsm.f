c! this is bdsm ----> blob detection and source measurement
c! called by noise.f and in turns calls menu of stuff and then goes back to noise.f
c! to look at results etc

        subroutine bdsm
        implicit none
        character opt*4,err*1
        character scratch*500,keyword*500,dir*500,fg*500
        character extn*10,comment*500,srldir*500
        real*8 keyvalue

        call writemenubdsm

        write (*,*)
        write (*,*) '  You are now inside the package "BDSM"'
        write (*,*) '  Blob Detection and Source Measurement'
        write (*,*) '  Quitting will take you back to  noise'
        write (*,*) '                            ---- R.N.M.'
333     continue
        write (*,*)    
        write (*,'(a20,$)') '   BDSM_Option ---> '
        read (*,*) opt
        call checkoptbdsm(opt,err)
        if (err.eq.'y') goto 333

        fg="paradefine"
        extn=""
        dir="./"
        keyword="scratch"
        call get_keyword(fg,extn,keyword,scratch,keyvalue,
     /    comment,"s",dir)
        keyword="srldir"
        call get_keyword(fg,extn,keyword,srldir,keyvalue,
     /    comment,"s",dir)

        if (opt.eq.'list') call calllib(scratch)
        if (opt.eq.'edit') call callie(scratch)
        if (opt.eq.'form') call callfi(scratch)
        if (opt.eq.'fiti') call callif(scratch)
        if (opt.eq.'make') call callil(scratch)
        if (opt.eq.'expe') call callexb
        if (opt.eq.'resi') call callre(scratch,srldir)
        if (opt.eq.'plot') call callpl(srldir)
        if (opt.eq.'asrl') call callas(scratch)
        if (opt.eq.'pasl') call callpasl(scratch)
        if (opt.eq.'prep') call callprep(scratch)
        if (opt.eq.'rmsd') call call_rmsd(scratch)
        if (opt.eq.'quit') call quitbdsm
        
        if (opt.ne.'quit') goto 333

        end
c!
c!
c!     ##################    START  OF  SUBROUTINES    ##################
c!
c!
        subroutine writemenubdsm
        implicit none

c! options are list quit form make edit fiti expe plot
        call system('rm -f a')
        open(unit=21,file='a',status='unknown')
        write (21,*) 
        write (21,*) 
        write (21,*) '  Type your favourite 4-letter word '
        write (21,*) 
        write (21,*) '  list : list images'
        write (21,*) '  expe : experiment'
        write (21,*) '  quit : quit now'
        write (21,*)   
        write (21,*) '  prep : pre-process'
        write (21,*) '  rmsd : calculate noise map'
        write (21,*) '  form : form islands'
        write (21,*) '  make : make island file'
        write (21,*) '  edit : edit island list'
        write (21,*) '  fiti : fit islands'
        write (21,*) '  resi : make residual image'
        write (21,*) '  plot : overplot sources/islands'
        write (21,*) '  asrl : associate source lists'
        write (21,*) '  pasl : plot associated sourcelists'
        close(21)
        call system('xterm -geometry 40x15 -T bdsm -hold -e cat a & ')

        return
        end


        subroutine callil(scratch)
        implicit none
        integer err
        character ipfile*500,file2*500
        integer n,m
        character scratch*500
        logical exists

444     continue
        write (*,*) '    imagefilename  islandmatrixfilename' 
        call system('rm -f a b')
        call getininp 
        call readininp(2,err)
        if (err.eq.1) goto 444
        open(unit=21,file='b',status='old')
         read (21,*) ipfile
         read (21,*) file2
        close(21)
        if (.not.exists(ipfile,scratch,'.img')) goto 444
        if (.not.exists(file2,scratch,'.rank')) goto 444
        call iland_mat2list(ipfile,file2,scratch)
        call system('rm -f a b')

        return
        end

        subroutine callfi(scratch)
        implicit none
        integer err
        character ipfile*500
        integer n,m
        character scratch*500
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
        if (.not.exists(ipfile,scratch,'.img')) goto 444
        call formislands(ipfile,scratch)
        call system('rm -f a b')

        return
        end

        subroutine calllib(scratch)
        implicit none
        integer err,nchar
        character str1*5000,scratch*500

        str1="ls -ltr "//scratch(1:nchar(scratch))//
     /       "*.img | grep -v total | grep -v fits | "
     /       //"awk '{print $9}' | sed 's/image\\///g'"
        call system(str1)

        return
        end

        subroutine callexb
        implicit none
        integer err
        character ipfile*500

444     continue
        write (*,*) '    imagename  2ndname'
        call system('rm -f a b')
        call getininp 
        call readininp(1,err)
        if (err.eq.1) goto 444
        open(unit=21,file='b',status='old')
         read (21,*) ipfile
        close(21)
        call experiment(ipfile)
        call system('rm -f a b')

        return
        end

        subroutine callif(scratch)
        implicit none
        integer err
        character ipfile*500,file2*500
        integer n,m
        character scratch*500
        logical exists

444     continue
        write (*,*) '    imagename  islandlistname'
        call system('rm -f a b')
        call getininp 
        call readininp(2,err)
        if (err.eq.1) goto 444
        open(unit=21,file='b',status='old')
         read (21,*) ipfile
         read (21,*) file2
        close(21)
        if (.not.exists(ipfile,scratch,'.img')) goto 444
        if (.not.exists(file2,scratch,'.islandlist')) goto 444
        call sourcemeasure(ipfile,file2,scratch)
        call system('rm -f a b')

        return
        end

        subroutine callre(scratch,srldir)
        implicit none
        integer err
        character ipfile*500
        real*8 sigma
        character scratch*500,srldir*500
        logical exists

444     continue
        write (*,*) '    sourcelistname (.gaul)'
        call system('rm -f a b')
        call getininp 
        call readininp(1,err)
        if (err.eq.1) goto 444
        open(unit=21,file='b',status='old')
         read (21,*) ipfile
        close(21)
        if (.not.exists(ipfile,srldir,'.gaul')) goto 444
        call make_src_residim(ipfile,'mv',srldir)
        call system('rm -f a b')

        return
        end

        subroutine callie(scratch)
        implicit none
        integer err
        character ipfile*500
        real*8 sigma
        character scratch*500
        logical exists

444     continue
        write (*,*) '    islandfilename'
        call system('rm -f a b')
        call getininp 
        call readininp(1,err)
        if (err.eq.1) goto 444
        open(unit=21,file='b',status='old')
         read (21,*) ipfile
        close(21)
        if (.not.exists(ipfile,scratch,'.islandlist')) goto 444
        call islandedit(ipfile,scratch)
        call system('rm -f a b')

        return
        end

        subroutine callpl(srldir)
        implicit none
        integer err
        character ipfile*500
        real*8 sigma
        character srldir*500
        logical exists

444     continue
        write (*,*) '    sourcelistname'
        call system('rm -f a b')
        call getininp 
        call readininp(1,err)
        if (err.eq.1) goto 444
        open(unit=21,file='b',status='old')
         read (21,*) ipfile
        close(21)
        if (.not.exists(ipfile,srldir,'.gaul')) goto 444
        call plotisland(ipfile)
        call system('rm -f a b')

        return
        end

        subroutine callas(scratch)
        implicit none
        integer err
        character f1*500,f2*500
        real*8 sigma
        character scratch*500
        logical exists

444     continue
        write (*,*) '    mastersourcelist  secondsourcelist'
        call system('rm -f a b')
        call getininp 
        call readininp(2,err)
        if (err.eq.1) goto 444
        open(unit=21,file='b',status='old')
         read (21,*) f1
         read (21,*) f2
        close(21)
        if (.not.exists(f1,scratch,'.gaul')) goto 444
        if (.not.exists(f2,scratch,'.gaul')) goto 444
        call associatesrl(f1,f2)
        call system('rm -f a b')

        return
        end

        subroutine callpasl(scratch)
        implicit none
        integer err,nchar
        character f1*100,cmd*500
        character scratch*500
        logical exists

444     continue
        write (*,*) '    AssociateSourceList Plotfiles are : '
        cmd="ls "//scratch(1:nchar(scratch))//
     /      "*.pasf > a; sed 's/image\\//     /g' a | "//
     /      "sed 's/.pasf//' > b; cat b"
        call system(cmd)
        write (*,*) '    Plotfilename '
        call system('rm -f a b')
        call getininp 
        call readininp(1,err)
        if (err.eq.1) goto 444
        open(unit=21,file='b',status='old')
         read (21,*) f1
        close(21)
        if (.not.exists(f1,scratch,'.pasf')) goto 444
        call plot_associatelist1(f1,scratch)
        call system('rm -f a b')

        return
        end

        subroutine callprep(scratch)
        implicit none
        integer err
        character f1*100
        character scratch*500
        logical exists

444     continue
        write (*,*) '    Inputimagename '
        call system('rm -f a b')
        call getininp 
        call readininp(1,err)
        if (err.eq.1) goto 444
        open(unit=21,file='b',status='old')
         read (21,*) f1
        close(21)
        if (.not.exists(f1,scratch,'.img')) goto 444
        call preprocess(f1,scratch)
        call system('rm -f a b')

        return
        end

        subroutine call_rmsd(scratch)
        implicit none
        integer err
        character f1*100
        character scratch*500
        logical exists

444     continue
        write (*,*) '    Inputimagename '
        call system('rm -f a b')
        call getininp 
        call readininp(1,err)
        if (err.eq.1) goto 444
        open(unit=21,file='b',status='old')
         read (21,*) f1
        close(21)
        if (.not.exists(f1,scratch,'.img')) goto 444
        call callrmsd(f1,scratch)
        call system('rm -f a b')

        return
        end

        subroutine checkoptbdsm(opt,err)
        implicit none
        integer nmenu
        parameter (nmenu=13)
        character opt*4,err*1,options(nmenu)*4
        integer i
        
        data options/'quit','list','fiti','make','edit','form','expe',
     /               'resi','plot','asrl','pasl','prep','rmsd'/

        err='y'
        do 100 i=1,nmenu
         if (opt.eq.options(i)) err='n'
100     continue
        
        return
        end


        subroutine quitbdsm
        implicit none
        character str1*500

        str1='ps -ef|grep xterm|grep geomet|grep bdsm|'//
     /       'awk Z{print "kill -9 " $2}Z > g ; source g'
        str1(45:45)="'"
        str1(67:67)="'"
        call system(str1)
        write (*,*) 
        write (*,*) '  Hope you enjoyed your BDSM session'
        write (*,*) '  Going back to "noise" now    ..... '

        return
        end

        

