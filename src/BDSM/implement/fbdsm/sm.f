c! this is bdsm ----> blob detection and source measurement
c! called by noise.f and in turns calls menu of stuff and then goes back to noise.f
c! to look at results etc

        subroutine sm(scratch,srldir,runcode)
        implicit none
        character opt*4,err*1
        character scratch*500,keyword*500,dir*500,fg*500
        character extn*20,comment*500,srldir*500,runcode*2
        real*8 keyvalue
        integer error,nchar

cf2py   intent(in) scratch,srldir,runcode

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

        if (opt.eq.'list') call calllib(scratch)
        if (opt.eq.'edit') call callie(scratch)
        if (opt.eq.'form') call callfi(scratch,runcode)
        if (opt.eq.'csrl') call callcsrl(scratch,srldir)
        if (opt.eq.'fiti') call callif(scratch,runcode,srldir)
        if (opt.eq.'st2g') call callst2g(scratch,srldir,runcode)
        if (opt.eq.'cl2g') call callcl2g(scratch,srldir,runcode)
        if (opt.eq.'make') call callil(scratch,runcode)
        if (opt.eq.'expe') call callexb
        if (opt.eq.'resi') call callre(scratch,srldir,runcode)
        if (opt.eq.'plot') call callpl(srldir)
        if (opt.eq.'asrl') call callas(scratch,srldir)
        if (opt.eq.'pasl') call callpasl(scratch)
        if (opt.eq.'aasl') call callaasl(scratch,srldir)
        if (opt.eq.'prep') call callprep(scratch,runcode)
        if (opt.eq.'rmsd') call call_rmsd(scratch,runcode)
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

c! options are list quit form make edit fiti expe plot st2g csrl cl2g
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
        write (21,*) '  csrl : combine source lists'
        write (21,*) '  pasl : plot associated sourcelists'
        write (21,*) '  aasl : analyse associated sourcelists'
        write (21,*) '  st2g : convert star to gaul and srl'
        write (21,*) '  cl2g : convert catalogue to gaul and srl'
        close(21)
        call system('xterm -geometry 40x25 -T bdsm -hold -e cat a & ')

        return
        end


        subroutine callil(scratch,runcode)
        implicit none
        integer err
        character ipfile*500,file2*500
        integer n,m
        character scratch*500,runcode
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
        call iland_mat2list(ipfile,file2,runcode,scratch)
        call system('rm -f a b')

        return
        end

        subroutine callcsrl(scratch,srldir)
        implicit none
        integer err
        character opfile*500,srldir*2
        integer n,m,nchar
        character scratch*500,solnname*500
        logical exists

444     continue
        write (*,*) '    outputfilename'
        call system('rm -f a b')
        call getininp 
        call readininp(1,err)
        if (err.eq.1) goto 444
        open(unit=21,file='b',status='old')
         read (21,*) opfile
        close(21)
        call combinesrl(opfile,scratch,srldir)
        call system('rm -f a b')

        return
        end

        subroutine callfi(scratch,runcode)
        implicit none
        integer err
        character ipfile*500,runcode*2,f2*500
        integer n,m,nchar
        character scratch*500,solnname*500
        logical exists

444     continue
        write (*,*) '    imagefilename   solnname' 
        call system('rm -f a b')
        call getininp 
        call readininp(2,err)
        if (err.eq.1) goto 444
        open(unit=21,file='b',status='old')
         read (21,*) ipfile
         read (21,*) solnname
        close(21)
        f2=ipfile(1:nchar(ipfile))//solnname(1:nchar(solnname))
        if (.not.exists(ipfile,scratch,'.img')) goto 444
        call formislands(ipfile,f2,runcode,scratch)
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

        subroutine callif(scratch,runcode,srldir)
        implicit none
        integer err
        character ipfile*500,file2*500,srldir*500
        integer n,m,nchar
        character scratch*500,runcode*2,solnname*500
        logical exists

444     continue
        write (*,*) '    imagename  solnname'
        call system('rm -f a b')
        call getininp 
        call readininp(2,err)
        if (err.eq.1) goto 444
        open(unit=21,file='b',status='old')
         read (21,*) ipfile
         read (21,*) solnname
        close(21)
        file2=ipfile(1:nchar(ipfile))//solnname(1:nchar(solnname))
        if (.not.exists(ipfile,scratch,'.img')) goto 444
        if (.not.exists(file2,scratch,'.islandlist')) goto 444
        call sourcemeasure(ipfile,file2,srldir,runcode,scratch)
        call system('rm -f a b')

        return
        end

        subroutine callre(scratch,srldir,runcode)
        implicit none
        integer err,nchar,n,m,l,l0
        character ipfile*500,runcode*2,imagename*500,solnname*500
        real*8 sigma
        character scratch*500,srldir*500,extn*20
        logical exists

444     continue
        write (*,*) '    imagename solnname (.gaul)'
        call system('rm -f a b')
        call getininp 
        call readininp(2,err)
        if (err.eq.1) goto 444
        open(unit=21,file='b',status='old')
         read (21,*) imagename
         read (21,*) solnname
        close(21)
        ipfile=imagename(1:nchar(imagename))//'.'//
     /         solnname(1:nchar(solnname))
        if (.not.exists(ipfile,srldir,'.gaul')) goto 444
        extn='.img'
        call readarraysize(imagename,extn,n,m,l)
        l0=1
        imagename=ipfile(1:nchar(ipfile))//'.ch0'
        call make_src_residim(ipfile,imagename,runcode,srldir,
     /             n,m,l,l0)
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

        subroutine callas(scratch,srldir)
        implicit none
        integer err
        character f1*500,f2*500,extn*20
        real*8 sigma,tol
        character scratch*500,srldir*500,rcode*2
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
        extn='.gaul'
        if (.not.exists(f1,srldir,extn)) goto 444
        if (.not.exists(f2,srldir,extn)) goto 444
        rcode='v '
        tol=0.d0
        call associatesrl(f1,f2,scratch,srldir,rcode,tol)
        call system('rm -f a b')

        return
        end

        subroutine callaasl(scratch,srldir)
        implicit none
        integer err,nchar
        character f1*500,cmd*500,srldir*500
        character scratch*500,extn*20
        logical exists

444     continue
        write (*,*) '    AssociateSourceList Plotfiles are : '
        cmd="ls -tr "//scratch(1:nchar(scratch))//
     /      "*.pasf | sed 's/.pasf//' > b; cat b"
        call system(cmd)
        write (*,*) '    Plotfilename '
        call system('rm -f a b')
        call getininp 
        call readininp(1,err)
        if (err.eq.1) goto 444
        open(unit=21,file='b',status='old')
         read (21,*) f1
        close(21)
        extn='.pasf'
        if (.not.exists(f1,scratch,extn)) goto 444
        call analyse_asrl(f1,scratch,srldir)
        call system('rm -f a b')

        return
        end

        subroutine callpasl(scratch)
        implicit none
        integer err,nchar
        character f1*500,cmd*500,extn*20
        character scratch*500
        logical exists

444     continue
        write (*,*) '    AssociateSourceList Plotfiles are : '
        cmd="ls -tr "//scratch(1:nchar(scratch))//
     /      "*.pasf | sed 's/.pasf//' > b; cat b"
        call system(cmd)
        write (*,*) '    Plotfilename '
        call system('rm -f a b')
        call getininp 
        call readininp(1,err)
        if (err.eq.1) goto 444
        open(unit=21,file='b',status='old')
         read (21,*) f1
        close(21)
        extn='.pasf'
        if (.not.exists(f1,scratch,extn)) goto 444
        call plot_associatelist1(f1,scratch)
        call system('rm -f a b')

        return
        end

        subroutine callcl2g(scratch,srldir,runcode)
        implicit none
        integer err
        character f1*500,runcode*2,f2*500,ch1
        character scratch*500,srldir*500,extn*20
        logical exists

444     continue
        write (*,*) '    First copy the read-file to readcat.f'
        write (*,*) '    and recompile noise.'
        write (*,*) '    Template file : template_call_cat2gaulsrl.f'
        write (*,'(a,$)') '    Done (y/n) : '
        read (*,*) ch1
        if (ch1.eq.'y') call call_cat2gaulsrl(scratch,srldir,runcode)

        return
        end

        subroutine callst2g(scratch,srldir,runcode)
        implicit none
        integer err
        character f1*500,runcode*2,f2*500
        character scratch*500,srldir*500,extn*20
        logical exists

444     continue
        write (*,*) '    name  file.solnnname '//
     /              '(name.star --> file.solnname.gaul'
        call system('rm -f a b')
        call getininp 
        call readininp(2,err)
        if (err.eq.1) goto 444
        open(unit=21,file='b',status='old')
         read (21,*) f1
         read (21,*) f2
        close(21)
        extn='.star'
        if (.not.exists(f1,srldir,extn)) goto 444
        extn='.header'
        if (.not.exists(f2,srldir,extn)) goto 444
        call star2gaulsrl(f1,f2,runcode,scratch,srldir,'v')
        call system('rm -f a b')

        return
        end

        subroutine callprep(scratch,runcode)
        implicit none
        integer err
        character f1*500,runcode*2
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
        call preprocess(f1,runcode,scratch)
        call system('rm -f a b')

        return
        end

        subroutine call_rmsd(scratch,runcode)
        implicit none
        integer err,nchar
        character f1*500,solnname*500,f2*500
        character scratch*500,runcode*2
        logical exists

444     continue
        write (*,*) '    Inputimagename  solnname'
        call system('rm -f a b')
        call getininp 
        call readininp(2,err)
        if (err.eq.1) goto 444
        open(unit=21,file='b',status='old')
         read (21,*) f1
         read (21,*) solnname
        close(21)
        f2=f1(1:nchar(f1))//solnname(1:nchar(solnname))
        if (.not.exists(f1,scratch,'.img')) goto 444
        call callrmsd(f1,f2,runcode,scratch)
        call system('rm -f a b')

        return
        end

        subroutine checkoptbdsm(opt,err)
        implicit none
        integer nmenu
        parameter (nmenu=17)
        character opt*4,err*1,options(nmenu)*4
        integer i
        
        data options/'quit','list','fiti','make','edit','form','expe',
     /               'resi','plot','asrl','pasl','prep','rmsd','aasl',
     /               'st2g','csrl','cl2g'/

        err='y'
        do 100 i=1,nmenu
         if (opt.eq.options(i)) err='n'
100     continue
        
        return
        end


        subroutine quitbdsm
        implicit none
        character str1*500

        str1='ps -ax|grep xterm|grep geomet|grep bdsm|'//
     /       'awk Z{print "kill -9 " $1}Z > g ; source g'
        str1(45:45)="'"
        str1(67:67)="'"
        call system(str1)
        write (*,*) 
        write (*,*) '  Hope you enjoyed your BDSM session'
        write (*,*) '  Going back to "noise" now    ..... '

        return
        end

        

