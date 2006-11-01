c! This is a suite of very basic image processing tasks. Creates noise images, source
c! images, and operates on them. 
c! This evolves into a prototype for testing stuff, to be later coverted into c++.
c!     ------ R. Niruj Mohan, 2005, Sterrewacht, Leiden.


        implicit none
        integer m,n,seed
        character opt*2,err*1
        character scratch*500,keyword*500,dir*500,fg*500
        character extn*10,comment*500
        real*8 keyvalue
        character fitsdir*500,fitsname*500,srldir*500
        character solnname*500,runcode*2
        integer nchar,error
        
        call writemenu

c! generate seed to pass on to call for random numbers
        call getseed(seed)

        write (*,*)
        write (*,*) '  This is the package "noise"'
        write (*,*) '  All output images are saved'
        write (*,*) '  Enter inputs in single line '
        write (*,*) '                  ---- R.N.M.'
333     continue
        write (*,*)    
        write (*,'(a15,$)') '   Option ---> '
        read (*,*) opt
        call checkopt(opt,err)
        if (err.eq.'y') goto 333

        runcode='mv'
        fg="paradefine"
        extn=""
        dir="./"
        keyword="scratch"
        call get_keyword(fg,extn,keyword,scratch,keyvalue,
     /    comment,"s",dir)
        keyword="fitsdir"
        call get_keyword(fg,extn,keyword,fitsdir,keyvalue,
     /    comment,"s",dir)
        if (fitsdir(nchar(fitsdir):nchar(fitsdir)).ne.'/') 
     /      fitsdir(nchar(fitsdir)+1:nchar(fitsdir)+1)='/'
        if (scratch(nchar(scratch):nchar(scratch)).ne.'/') 
     /      scratch(nchar(scratch)+1:nchar(scratch)+1)='/'

        if (opt.eq.'li') call callli(scratch)
        if (opt.eq.'la') call callla(scratch)
        if (opt.eq.'cc') call callcc(scratch)
        if (opt.eq.'fg') call callfg(scratch)
        if (opt.eq.'ex') call callex(scratch)
        if (opt.eq.'tr') call calltr(scratch)
        if (opt.eq.'sl') call callsl(scratch)
        if (opt.eq.'pp') call callpp(scratch)
        if (opt.eq.'ph') call callph(scratch)
        if (opt.eq.'sf') call callsf(scratch)
        if (opt.eq.'da') call callda(scratch)
        if (opt.eq.'cn') call callcn(seed,scratch)
        if (opt.eq.'gp') call callgp(scratch)
        if (opt.eq.'ff') call callff(scratch,fitsdir)
        if (opt.eq.'sm') call callsm(scratch)
        if (opt.eq.'nb') call callnb(scratch)
        if (opt.eq.'ri') call callri(scratch)
        if (opt.eq.'cg') call callcg(scratch)
        if (opt.eq.'ta') call callta(scratch,seed)
        if (opt.eq.'cl') call callcl(scratch)
        if (opt.eq.'cs') call callcs(seed,scratch)
        if (opt.eq.'rf') call callrf(fitsdir,scratch,runcode)
        if (opt.eq.'dd') call calldd(scratch)
        if (opt.eq.'mo') call callmo(scratch)
        if (opt.eq.'io') call callio(scratch)
        if (opt.eq.'cm') call callcm(seed,scratch)
        if (opt.eq.'mi') call callmi(scratch)
        if (opt.eq.'mf') call callmf(scratch)
        if (opt.eq.'mn') call callmn(scratch)
        if (opt.eq.'dt') call calldt(scratch)
        if (opt.eq.'ac') call callac(scratch)
        if (opt.eq.'di') call calldi(scratch)
        if (opt.eq.'ps') call callps(seed)
        if (opt.eq.'ci') call callci(scratch)
        if (opt.eq.'is') call callis(scratch)
        if (opt.eq.'su') call callsu(scratch)
        if (opt.eq.'sc') call callsc(scratch)
        if (opt.eq.'pb') call callpb(scratch)
        if (opt.eq.'rh') call callrh(scratch)
        if (opt.eq.'ch') call callch(scratch)
        if (opt.eq.'qn') call quit
        
        if (opt.ne.'qn') goto 333

        end
c!
c!
c!     ##################    START  OF  SUBROUTINES    ##################
c!
c!
        subroutine writemenu
        implicit none

c! options are cn ri cg cm mi mf mn dt di qn ac li da ps ci is sc pb rh ch cc ph pp fg sl su io mo tr ex ff dd la nb sm gp rf cs cl ta
        call system('rm -f a')
        open(unit=21,file='a',status='unknown')
        write (21,*) 
        write (21,*) '  Type in bracketed letters as small'
        write (21,*) '  li : (l)ist (i)mages'
        write (21,*) '  la : (l)ist (a)ll'
        write (21,*) '  da : (d)elete (a)ll'
        write (21,*) '  rh : (r)ead (h)eader'
        write (21,*) '  ph : (p)ut (h)eader'
        write (21,*) '  ch : (c)opy (h)eader'
        write (21,*) '  ex : (ex)perimental'
        write (21,*) '  ff : (f)ile (f)ormat coversion'
        write (21,*) '  qn : (q)uit (n)ow'
        write (21,*)   
        write (21,*) '  cc : (c)reate (c)onstant image' 
        write (21,*) '  pp : (p)ut (p)ixels'
        write (21,*) '  gp : (g)et (p)ixels'
        write (21,*) '  dt : (d)isplay (t)wo images'
        write (21,*) '  tr : (tr)anspose image'
        write (21,*) '  di : (d)isplay (i)mage'
        write (21,*) '  ci : (c)ombine (i)mages'
        write (21,*) '  nb : (n)-(b)it'
        write (21,*) '  is : (i)mage (s)tatistics'
        write (21,*) '  io : (i)mage (o)perations'
        write (21,*) '  cl : (cl)ip image'
        write (21,*) '  mo : (mo)ment analysis'
        write (21,*) '  su : (su)bim'
        write (21,*) '  sl : (sl)ice plot'
        write (21,*) 
        write (21,*) '  cn : (c)reate (n)oise image '
        write (21,*) '  cg : (c)onvolve with (g)aussian'
        write (21,*) '  cs : (c)reate (s)ources'
        write (21,*) '  fg : (f)it (g)aussian'
        write (21,*) '  dd : (d)etect (d)iscontinuities'
        write (21,*) '  cm : (c)reate (m)odulation'
        write (21,*) '  rf : (r)ead in (f)its file'
        write (21,*) '  mi : (m)odulate (i)mage'
        write (21,*) '  mf : (m)edian (f)ilter'
        write (21,*) '  mn : (m)ake (n)oise map'
        write (21,*) '  ac : (a)uto (c)orrelation of image'
        write (21,*) '  sf : (s)tructure (f)unction'
        write (21,*) '  ps : (p)opulate (s)ources'
        write (21,*) '  sc : n-(s)igma (c)ut'
        write (21,*) '  pb : (p)rimary (b)eam correction'
        write (21,*) 
        write (21,*) '  sm : (s)ource (m)easurement algorithms'
        write (21,*) '  ta : (t)est (a)lgorithms'
        write (21,*) '  Shapelet algorithms in ../shapelets'
        close(21)
        call system('xterm -geometry 45x45 -hold -e cat a & ')

        return
        end


        subroutine callcn(seed,scratch)
        implicit none
        integer err
        character ipfile*500,scratch*500
        real*8 sigmaJy
        integer n,m,seed
        
444     continue
        write (*,*) '    imagefilename  xsize  ysize  sigma(Jy)'
        call system('rm -f a b')
        call getininp 
        call readininp(4,err)
        if (err.eq.1) goto 444
        open(unit=21,file='b',status='old')
         read (21,*) ipfile
         read (21,*) n
         read (21,*) m
         read (21,*) sigmaJy
        close(21)
        call cr8noisemap(ipfile,n,m,sigmaJy,seed,scratch)
        call system('rm -f a b')

        return
        end

        subroutine callsm(scratch)
        implicit none
        character scratch*500

        call bdsm

        return
        end

        subroutine callri(scratch)
        implicit none
        integer err
        character ipfile*500,opfile*500
        character scratch*500
        integer n,m

444     continue
        write (*,*) '    imagefilename opfilename n m'
        call system('rm -f a b')
        call getininp 
        call readininp(4,err)
        if (err.eq.1) goto 444
        open(unit=21,file='b',status='old')
         read (21,*) ipfile
         read (21,*) opfile
         read (21,*) n
         read (21,*) m
        close(21)
        call readinimage(ipfile,opfile,n,m,scratch)
        call system('rm -f a b')

        return
        end

        subroutine callpp(scratch)
        implicit none
        integer err
        character ipfile*500,opfile*500,code*3
        character scratch*500
        logical exists

444     continue
        write (*,*) '    imagefilename  outputfilename  add/put'
        call system('rm -f a b')
        call getininp 
        call readininp(3,err)
        if (err.eq.1) goto 444
        open(unit=21,file='b',status='old')
         read (21,*) ipfile
         read (21,*) opfile
         read (21,*) code
        close(21)
        if (.not.exists(ipfile,scratch,'.img')) goto 444
        call callputpixel(ipfile,opfile,code)
        call system('rm -f a b')

        return
        end

        subroutine callgp(scratch)
        implicit none
        integer err
        character ipfile*500,code*2
        character scratch*500
        logical exists

444     continue
        write (*,*) '    imagefilename  tv/wr'
        call system('rm -f a b')
        call getininp 
        call readininp(2,err)
        if (err.eq.1) goto 444
        open(unit=21,file='b',status='old')
         read (21,*) ipfile
         read (21,*) code
        close(21)
        if (.not.exists(ipfile,scratch,'.img')) goto 444
        call callgetpixel(ipfile,code)
        call system('rm -f a b')

        return
        end

        subroutine callcc(scratch)
        implicit none
        integer err
        character ipfile*500
        character scratch*500
        real*8 value
        integer sx,sy

444     continue
        write (*,*) '    imagefilename  sizex  sizey  constantval (Jy)'
        call system('rm -f a b')
        call getininp 
        call readininp(4,err)
        if (err.eq.1) goto 444
        open(unit=21,file='b',status='old')
         read (21,*) ipfile
         read (21,*) sx
         read (21,*) sy
         read (21,*) value
        close(21)
        call cr8constantim(ipfile,sx,sy,value)
        call system('rm -f a b')

        return
        end

        subroutine callnb(scratch)
        implicit none
        integer err
        character ipfile*500
        real*8 sigma
        character scratch*500
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
        if (.not.exists(ipfile,scratch,'.img')) goto 444
        call nbit(ipfile,scratch)
        call system('rm -f a b')

        return
        end

        subroutine callff(scratch,fitsdir)
        implicit none
        integer err
        character ipfile*500,scratch*500,fitsdir*500
        real*8 sigma

444     continue
        write (*,*) '    imagefilename ' 
        call system('rm -f a b')
        call getininp 
        call readininp(1,err)
        if (err.eq.1) goto 444
        open(unit=21,file='b',status='old')
         read (21,*) ipfile
        close(21)
        call fileformat(ipfile,scratch,fitsdir)
        call system('rm -f a b')

        return
        end

        subroutine calldd(scratch)
        implicit none
        integer err
        character ipfile*500,opfile*500
        character scratch*500
        logical exists

444     continue
        write (*,*) '    inputfilename  opfilename'
        call system('rm -f a b')
        call getininp 
        call readininp(2,err)
        if (err.eq.1) goto 444
        open(unit=21,file='b',status='old')
         read (21,*) ipfile
         read (21,*) opfile
        close(21)
        if (.not.exists(ipfile,scratch,'.img')) goto 444
        call detectdiscont(ipfile,opfile)
        call system('rm -f a b')

        return
        end

        subroutine callta(scratch,seed)
        implicit none
        integer seed
        character scratch*500

        call calltest(scratch,seed)

        return
        end

        subroutine callcg(scratch)
        implicit none
        integer err
        character ipfile*500,opfile*500,filter*4
        character scratch*500
        logical exists

444     continue
        write (*,*) '    inputname  outputname  filter (gaus/boxf)'
        call system('rm -f a b')
        call getininp 
        call readininp(3,err)
        if (err.eq.1) goto 444
        open(unit=21,file='b',status='old')
         read (21,*) ipfile
         read (21,*) opfile
         read (21,*) filter
        close(21)
        if (.not.exists(ipfile,scratch,'.img')) goto 444
        call convolveimage(ipfile,opfile,filter,scratch)
        call system('rm -f a b')

        return
        end

        subroutine callcl(scratch)
        implicit none
        integer err
        character ipfile*500,opfile*500
        character scratch*500
        logical exists

444     continue
        write (*,*) '    inputfilename  outputfilename'
        call system('rm -f a b')
        call getininp 
        call readininp(2,err)
        if (err.eq.1) goto 444
        open(unit=21,file='b',status='old')
         read (21,*) ipfile
         read (21,*) opfile
        close(21)
        if (.not.exists(ipfile,scratch,'.img')) goto 444
        call clip(ipfile,opfile)
        call system('rm -f a b')

        return
        end

        subroutine callcs(seed,scratch)
        implicit none
        integer err,seed
        character ipfile*500,scratch*500
        real*8 sigma

444     continue
        write (*,*) '    outputfilename'
        call system('rm -f a b')
        call getininp 
        call readininp(1,err)
        if (err.eq.1) goto 444
        open(unit=21,file='b',status='old')
         read (21,*) ipfile
        close(21)
        call cr8sources(ipfile,seed,scratch)
        call system('rm -f a b')

        return
        end

        subroutine callli(scratch)
        implicit none
        integer err,nchar
        character str1*5000
        character scratch*500

        str1="ls -ltr "//scratch(1:nchar(scratch))//
     /       "*.img | grep -v total | grep -v fits | "
     /       //"awk '{print $9}' | sed 's/image\\///g'"
        call system(str1)

        return
        end

        subroutine callla(scratch)
        implicit none
        integer err,nchar
        character scratch*500
        character str1*5000

c        str1="ls -tr image/ | grep -v total | grep -v fits > a ;"
c     /       //"sed 's/^/sed 1q image\\//g' a > b;source b > c;"
c     /       //"paste a c "
c     /       //" |sed 's/image\\//       /g'"
        str1="ls -ltr "//scratch(1:nchar(scratch))//
     /    " | grep -v total | grep -v fits | awk '{print $9}'"
        call system(str1)

        return
        end

        subroutine callda(scratch)
        implicit none
        integer err,nchar
        character scratch*500
        character str1*500

        str1="rm -f "//scratch(1:nchar(scratch))//"*"
        call system(str1)

        return
        end

        subroutine callex(scratch)
        implicit none
        character scratch*500
        integer err
        character f1*500,f2*500

444     continue
        write (*,*) '    imagename  2ndname'
        call system('rm -f a b')
        call getininp 
        call readininp(1,err)
        if (err.eq.1) goto 444
        open(unit=21,file='b',status='old')
         read (21,*) f1
        close(21)
        call experiment(f1)
        call system('rm -f a b')

        return
        end

        subroutine callcm(seed,scratch)
        implicit none
        integer err
        integer n,m,seed
        character opfile*500,scratch*500

444     continue
        write (*,*) '    opfilename  xsize  ysize'
        call system('rm -f a b')
        call getininp 
        call readininp(3,err)
        if (err.eq.1) goto 444
        open(unit=21,file='b',status='old')
         read (21,*) opfile
         read (21,*) n
         read (21,*) m
        close(21)
        call cr8modulationimage(opfile,n,m,seed,scratch)
        call system('rm -f a b')

        return
        end

        subroutine callfg(scratch)
        implicit none
        integer err
        character imf*500
        character scratch*500
        logical exists

444     continue
        write (*,*) '    imagefile  '
        call system('rm -f a b')
        call getininp 
        call readininp(1,err)
        if (err.eq.1) goto 444
        open(unit=21,file='b',status='old')
         read (21,*) imf
        close(21)
        if (.not.exists(imf,scratch,'.img')) goto 444
        call callfitgaussian(imf)
        call system('rm -f a b')

        return
        end

        subroutine callmi(scratch)
        implicit none
        integer err
        character imf*500,modf*500,opf*500
        character scratch*500
        logical exists

444     continue
        write (*,*) '    imagefile  modulatefile  opfile'
        call system('rm -f a b')
        call getininp 
        call readininp(3,err)
        if (err.eq.1) goto 444
        open(unit=21,file='b',status='old')
         read (21,*) imf
         read (21,*) modf
         read (21,*) opf
        close(21)
        if (.not.exists(imf,scratch,'.img')) goto 444
        if (.not.exists(modf,scratch,'.img')) goto 444
        call modulate(imf,modf,opf,scratch)
        call system('rm -f a b')

        return
        end

        subroutine callmf(scratch)
        implicit none
        integer err
        integer ft
        character ipfile*500,opfile*500
        character scratch*500
        logical exists

444     continue
        write (*,*) '    imagefile  opfile  filtersize'
        call system('rm -f a b')
        call getininp 
        call readininp(3,err)
        if (err.eq.1) goto 444
        open(unit=21,file='b',status='old')
         read (21,*) ipfile
         read (21,*) opfile
         read (21,*) ft
        close(21)
        if (mod(ft,2).eq.0) then
         ft=ft+1
         write (*,*) ' Increasing filter size to ',ft
        end if
        if (.not.exists(ipfile,scratch,'.img')) goto 444
        call medianfilter(ft,ipfile,opfile)
        call system('rm -f a b')

        return
        end

        subroutine callmn(scratch)
        implicit none
        integer err
        character ipfile*500,mean*500,std*500,median*500
        character scratch*500
        logical exists

444     continue
        write (*,*) '    inputfile  meanfile  rmsfile medianfile'
        call system('rm -f a b')
        call getininp 
        call readininp(4,err)
        if (err.eq.1) goto 444
        open(unit=21,file='b',status='old')
         read (21,*) ipfile
         read (21,*) mean
         read (21,*) std
         read (21,*) median
        close(21)
        if (.not.exists(ipfile,scratch,'.img')) goto 444
        call boxnoiseimage(ipfile,mean,std,median)
        call system('rm -f a b')

        return
        end

        subroutine callmo(scratch)
        implicit none
        integer err
        character f1*500
        character scratch*500
        logical exists

444     continue
        write (*,*) '    inputfile'
        call system('rm -f a b')
        call getininp 
        call readininp(1,err)
        if (err.eq.1) goto 444
        open(unit=21,file='b',status='old')
         read (21,*) f1
        close(21)
        if (.not.exists(f1,scratch,'.img')) goto 444
        call callmoment(f1)
        call system('rm -f a b')

        return
        end

        subroutine calltr(scratch)
        implicit none
        integer err
        character f1*500,f2*500
        character scratch*500
        logical exists

444     continue
        write (*,*) '    inputimage  outputimage'
        call system('rm -f a b')
        call getininp 
        call readininp(2,err)
        if (err.eq.1) goto 444
        open(unit=21,file='b',status='old')
         read (21,*) f1
         read (21,*) f2
        close(21)
        if (.not.exists(f1,scratch,'.img')) goto 444
        call transposeim(f1,f2)
        call system('rm -f a b')

        return
        end

        subroutine calldt(scratch)
        implicit none
        integer err
        character f1*500,f2*500
        character scratch*500
        logical exists

444     continue
        write (*,*) '    file1  file'
        call system('rm -f a b')
        call getininp 
        call readininp(2,err)
        if (err.eq.1) goto 444
        open(unit=21,file='b',status='old')
         read (21,*) f1
         read (21,*) f2
        close(21)
        if (.not.exists(f1,scratch,'.img')) goto 444
        if (.not.exists(f2,scratch,'.img')) goto 444
        call displaytwoim(f1,f2)
        call system('rm -f a b')

        return
        end

        subroutine calldi(scratch)
        implicit none
        integer err
        character f1*500
        character scratch*500
        logical exists

444     continue
        write (*,*) '    imagefile'
        call system('rm -f a b')
        call getininp 
        call readininp(1,err)
        if (err.eq.1) goto 444
        open(unit=21,file='b',status='old')
         read (21,*) f1
        close(21)
        if (.not.exists(f1,scratch,'.img')) goto 444
        call displayim(f1,scratch)
        call system('rm -f a b')

        return
        end

        subroutine callac(scratch)
        implicit none
        integer err
        character f1*500,f2*500
        character scratch*500
        logical exists

444     continue
        write (*,*) '    inputfile  opfile'
        call system('rm -f a b')
        call getininp 
        call readininp(2,err)
        if (err.eq.1) goto 444
        open(unit=21,file='b',status='old')
         read (21,*) f1
         read (21,*) f2
        close(21)
        if (.not.exists(f1,scratch,'.img')) goto 444
        call acf(f1,f2)
        call system('rm -f a b')

        return
        end

        subroutine callsf(scratch)
        implicit none
        integer err
        character f1*500,f2*500
        integer order
        character scratch*500
        logical exists

444     continue
        write (*,*) '    inputfile  opfile  order'
        call system('rm -f a b')
        call getininp 
        call readininp(3,err)
        if (err.eq.1) goto 444
        open(unit=21,file='b',status='old')
         read (21,*) f1
         read (21,*) f2
         read (21,*) order
        close(21)
        if (.not.exists(f1,scratch,'.img')) goto 444
        call structfn(f1,f2,order)
        call system('rm -f a b')

        return
        end


        subroutine callps(seed)
        implicit none
        integer err
        character f1*500
        integer seed

444     continue
        write (*,*) '    filename'
        call system('rm -f a b')
        call getininp 
        call readininp(1,err)
        if (err.eq.1) goto 444
        open(unit=21,file='b',status='old')
         read (21,*) f1
        close(21)
        call populatesrcs(f1,seed)
        call system('rm -f a b')

        return
        end

        subroutine callci(scratch)
        implicit none
        integer err
        character f1*500,f2*500,f3*500,str1*3
        character scratch*500
        logical exists

444     continue
        write (*,*) '    image1  image2  outputimage  oper' 
        write (*,*) '        (oper = add/sub/mul/div) '
        call system('rm -f a b')
        call getininp 
        call readininp(4,err)
        if (err.eq.1) goto 444
        open(unit=21,file='b',status='old')
         read (21,*) f1
         read (21,*) f2
         read (21,*) f3
         read (21,*) str1
        close(21)
        if (str1.ne.'add'.and.str1.ne.'mul'.and.str1.ne.'sub'.
     /      and.str1.ne.'div') goto 444
        if (.not.exists(f1,scratch,'.img')) goto 444
        if (.not.exists(f2,scratch,'.img')) goto 444
        call combineimage(f1,f2,f3,str1)
        call system('rm -f a b')

        return
        end

        subroutine callio(scratch)
        implicit none
        integer err
        character f1*500,f2*500
        character scratch*500
        logical exists

444     continue
        write (*,*) '    inputimage  outputimage'
        call system('rm -f a b')
        call getininp 
        call readininp(2,err)
        if (err.eq.1) goto 444
        open(unit=21,file='b',status='old')
         read (21,*) f1
         read (21,*) f2
        close(21)
        if (.not.exists(f1,scratch,'.img')) goto 444
        call operateimage(f1,f2)
        call system('rm -f a b')

        return
        end

        subroutine callsu(scratch)
        implicit none
        integer err
        character f1*500,f2*500,code*2
        character scratch*500
        logical exists

444     continue
        write (*,*) '    inputimage  outputimage  tv/wr'
        call system('rm -f a b')
        call getininp 
        call readininp(3,err)
        if (err.eq.1) goto 444
        open(unit=21,file='b',status='old')
         read (21,*) f1
         read (21,*) f2
         read (21,*) code
        close(21)
        if (.not.exists(f1,scratch,'.img')) goto 444
        call subim(f1,f2,code)
        call system('rm -f a b')

        return
        end

        subroutine callis(scratch)
        implicit none
        integer err
        character f1*500 
        real*8 kappa
        character scratch*500
        logical exists

444     continue
        write (*,*) '    image  k-sigma'
        call system('rm -f a b')
        call getininp 
        call readininp(2,err)
        if (err.eq.1) goto 444
        open(unit=21,file='b',status='old')
         read (21,*) f1
         read (21,*) kappa
        close(21)
        if (.not.exists(f1,scratch,'.img')) goto 444
        call imagestats(f1,kappa)
        call system('rm -f a b')

        return
        end

        subroutine callsc(scratch)
        implicit none
        integer err
        character f1*500,f2*500
        real*8 nsig
        character scratch*500
        logical exists

444     continue
        write (*,*) '    inputimage  outputimage  n(-sigma cut)'
        call system('rm -f a b')
        call getininp 
        call readininp(3,err)
        if (err.eq.1) goto 444
        open(unit=21,file='b',status='old')
         read (21,*) f1
         read (21,*) f2
         read (21,*) nsig
        close(21)
        if (.not.exists(f1,scratch,'.img')) goto 444
        call nsigmacut(f1,f2,nsig)
        call system('rm -f a b')

        return
        end

        subroutine callpb(scratch)
        implicit none
        integer err
        character f1*500,f2*500
        real*8 nsig
        character scratch*500
        logical exists

444     continue
        write (*,*) '    inputimage  outputimage'
        call system('rm -f a b')
        call getininp 
        call readininp(2,err)
        if (err.eq.1) goto 444
        open(unit=21,file='b',status='old')
         read (21,*) f1
         read (21,*) f2
        close(21)
        if (.not.exists(f1,scratch,'.img')) goto 444
        call primarybeam(f1,f2)
        call system('rm -f a b')

        return
        end

        subroutine callrh(scratch)
        implicit none
        integer err
        character f1*500
        real*8 nsig
        character scratch*500
        logical exists

444     continue
        write (*,*) '    inputimage'
        call system('rm -f a b')
        call getininp 
        call readininp(1,err)
        if (err.eq.1) goto 444
        open(unit=21,file='b',status='old')
         read (21,*) f1
        close(21)
        if (.not.exists(f1,scratch,'.img')) goto 444
        call readheader(f1,scratch)
        call system('rm -f a b')

        return
        end

        subroutine callch(scratch)
        implicit none
        integer err
        character f1*500,f2*500
        character scratch*500
        logical exists

444     continue
        write (*,*) '    image_copyfrom  image_copyto'
        call system('rm -f a b')
        call getininp 
        call readininp(2,err)
        if (err.eq.1) goto 444
        open(unit=21,file='b',status='old')
         read (21,*) f1
         read (21,*) f2
        close(21)
        if (.not.exists(f1,scratch,'.img')) goto 444
        call copyheader(f1,f2,scratch)
        call system('rm -f a b')

        return
        end


        subroutine callph(scratch)
        implicit none
        integer err
        character f1*500
        character scratch*500
        logical exists

444     continue
        write (*,*) '    imagename'
        call system('rm -f a b')
        call getininp 
        call readininp(1,err)
        if (err.eq.1) goto 444
        open(unit=21,file='b',status='old')
         read (21,*) f1
        close(21)
        if (.not.exists(f1,scratch,'.img')) goto 444
        call puthead(f1,scratch)
        call system('rm -f a b')

        return
        end

        subroutine callsl(scratch)
        implicit none
        integer err
        character f1*500
        character scratch*500
        logical exists

444     continue
        write (*,*) '    imagename'
        call system('rm -f a b')
        call getininp 
        call readininp(1,err)
        if (err.eq.1) goto 444
        open(unit=21,file='b',status='old')
         read (21,*) f1
        close(21)
        if (.not.exists(f1,scratch,'.img')) goto 444
        call slice(f1)
        call system('rm -f a b')

        return
        end

        subroutine callrf(fitsdir,scratch,runcode)
        implicit none
        integer err,nchar
        character filename*500,fitsdir*500,fitsname*500,runcode*2
        character scratch*500
        logical exists

444     continue
        write (*,*) '    FITSfilename '
        call system('rm -f a b')
        call getininp 
        call readininp(1,err)
        if (err.eq.1) goto 444
        open(unit=21,file='b',status='old')
         read (21,*) fitsname
        close(21)
        filename=''
        if (fitsname(nchar(fitsname)-4:nchar(fitsname)).eq.'.FITS'.or.
     /     fitsname(nchar(fitsname)-4:nchar(fitsname)).eq.'.fits') then
         filename=fitsname(1:nchar(fitsname)-5)
        else
         filename=fitsname
        end if
c        if (.not.exists(filename,scratch,'FITS)) goto 444
        call callreadfits(fitsdir,fitsname,filename,scratch,runcode)
        call system('rm -f a b')

        return
        end


        subroutine checkopt(opt,err)
        implicit none
        integer nmenu
        parameter (nmenu=41)
        character opt*2,err*1,options(nmenu)*2
        integer i
        
        data options/'cn','ri','cg','cm','mi','mf','mn','dt','qn',
     /               'di','ac','li','da','ps','ci','is','sc','pb',
     /               'rh','ch','cc','sf','ph','pp','fg','sl','su',
     /               'io','mo','tr','ex','ff','dd','la','nb','sm',
     /               'gp','rf','cs','cl','ta'/

        err='y'
        do 100 i=1,nmenu
         if (opt.eq.options(i)) err='n'
100     continue
        
        return
        end




        subroutine quit
        implicit none
        character str1*100

        str1='ps -ef|grep xterm|grep geomet|awk Z{print "kill -9 " '//
     /       '$2}Z > g ; source g'
        str1(35:35)="'"
        str1(57:57)="'"
        call system(str1)

        return
        end

