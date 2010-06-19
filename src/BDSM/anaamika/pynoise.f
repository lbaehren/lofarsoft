
        implicit none
        


        subroutine callcn(seed,scratch,srldir)
        write (*,*) '    imagefilename  xsize  ysize  sigma(Jy)'
        call cr8noisemap(ipfile,n,m,sigmaJy,seed,scratch,srldir)

        subroutine callsm(scratch,srldir,runcode)
        call sm(scratch,srldir,runcode)

        subroutine callsh(scratch,srldir,runcode)
        call shapelets(scratch,srldir,runcode)

        subroutine callri(scratch)
        write (*,*) '    imagefilename opfilename n m'
        call readinimage(ipfile,opfile,n,m,scratch)

        subroutine callpp(scratch)
        write (*,*) '    imagefilename  outputfilename  add/put'
        call callputpixel(ipfile,opfile,code)

        subroutine callgp(scratch)
        write (*,*) '    imagefilename  tv/wr'
        call callgetpixel(ipfile,code)

        subroutine callcc(scratch)
        write (*,*) '    imagefilename  sizex  sizey  constantval (Jy)'
        call cr8constantim(ipfile,sx,sy,value)

        subroutine callnb(scratch,srldir)
        write (*,*) '    imagefilename ' 
        call nbit(ipfile,scratch,srldir)

        subroutine callff(scratch,fitsdir)
        write (*,*) '    imagefilename ' 
        call fileformat(ipfile,scratch,fitsdir)

        subroutine calldd(scratch)
        write (*,*) '    inputfilename  opfilename'
        call detectdiscont(ipfile,opfile)

        subroutine callta(scratch,srldir,seed)
        call calltest(scratch,srldir,seed)

        subroutine callat(scratch,runcode,fitsdir)
        write (*,*) '    inputname  '
        call callmwt(ipfile,scratch,runcode,fitsdir)

        subroutine callfr(scratch)
        write (*,*) '    inputname  '
        call fn_radius(scratch,ipfile)

        subroutine callfs(scratch)
        write (*,*) '    inputname  '
        call fftshift2(ipfile,scratch)

        subroutine callms(scratch)
        write (*,*) '    inputname  sizeX   sizeY'
        call makeshapes(ipfile,n,m,scratch)

        subroutine callco(scratch,runcode)
        write (*,*) '    image  transferfunction  opfile ' 
        call callfftw(f1,f2,n,m,scratch,op,runcode)

        subroutine callcg(scratch,srldir)
        write (*,*) '    inputname  outputname  filter (gaus/boxf)'
        call convolveimage(ipfile,opfile,filter,scratch,srldir)

        subroutine callcl(scratch)
        write (*,*) '    inputfilename  outputfilename'
        call clip(ipfile,opfile)

        subroutine callcs(seed,scratch,srldir,fitsdir)
        write (*,*) '    outputfilename'
        call cr8sources(ipfile,seed,scratch,srldir,fitsdir)

        subroutine callli(scratch)
333     write (*,'(a,$)') '  (1) *.img  (2) *.gaul  (3) string : '
        read (*,*) i
        if (i.eq.3) then
         write (*,'(a,$)') '  String to search ? '
         read (*,*) str
         str='*'//str(1:nchar(str))//'*'
        end if
        if (i.eq.1) str='*.img'
        if (i.eq.2) str='*.gaul'
        str1="cd "//scratch(1:nchar(scratch))//"; ls -ltr "//
     /    str(1:nchar(str))//" | grep -v total | grep -v fits | "
     /    //"awk '{print $9}' | sed 's/image\\///g'"
        call system(str1)

        subroutine callla(scratch)
        str1="ls -ltr "//scratch(1:nchar(scratch))//
     /    " | grep -v total | grep -v fits | awk '{print $9}'"
        call system(str1)

        subroutine calldf(scratch)
333     write (*,'(a,$)') '   Delete (a)ll, (i)images, (f)ile ? '
        read (*,*) chr1
        if (chr1.eq.'a') then
         str1="rm -f "//scratch(1:nchar(scratch))//"*"
        if (chr1.eq.'i') then
         str1="rm -f "//scratch(1:nchar(scratch))//"*.img"
        if (chr1.eq.'f') then
         write (*,'(a,$)') '  Filename (with extn) : '
         read (*,*) str1
         str1="rm -f "//scratch(1:nchar(scratch))//str1(1:nchar(str1))
         call system(str1)

        subroutine callex(scratch,srldir)
        write (*,*) '   imagename  '
        call experiment(f1,scratch,srldir)

        subroutine callcm(seed,scratch,srldir)
        write (*,*) '    opfilename  xsize  ysize'
        call cr8modulationimage(opfile,n,m,seed,scratch,srldir)

        subroutine callfg(scratch)
        write (*,*) '    imagefile  '
        call callfitgaussian(imf)

        subroutine callmi(scratch,srldir)
        write (*,*) '    imagefile  modulatefile  opfile'
        call modulate(imf,modf,opf,scratch,srldir)

        subroutine callmf(scratch)
        write (*,*) '    imagefile  opfile  filtersize'
        if (mod(ft,2).eq.0) then
         ft=ft+1
         write (*,*) '  Increasing filter size to ',ft
        end if
        call medianfilter(ft,ipfile,opfile)

        subroutine callmn(scratch)
        write (*,*) '    inputfile  meanfile  rmsfile medianfile'
        call boxnoiseimage(ipfile,mean,std,median)

        subroutine callmo(scratch)
        write (*,*) '    inputfile'
        call callmoment(f1)

        subroutine calltr(scratch)
        write (*,*) '    inputimage  outputimage'
        call transposeim(f1,f2)

        subroutine calldt(scratch)
        write (*,*) '    file1  file'
        call displaytwoim(f1,f2)

        subroutine calldi(scratch,srldir)
        write (*,*) '    imagefile'
        call displayim(f1,scratch,srldir)

        subroutine callac(scratch)
        write (*,*) '    inputfile  opfile'
        call acf(f1,f2)

        subroutine callsf(scratch)
        write (*,*) '    inputfile  opfile  order'
        call structfn(f1,f2,order)

        subroutine callps(seed,scratch,srldir)
        write (*,*) '    filename'
        call populatesrcs(f1,seed,scratch,srldir)

        subroutine callai(scratch,runcode)
        write (*,*) '    outputfilename'
        call addimages(scratch,opname,runcode)

        subroutine callci(scratch)
        write (*,*) '    image1  image2  outputimage  oper' 
        write (*,*) '        (oper = add/sub/mul/div) '
        call combineimage(f1,f2,f3,str1)

        subroutine callio(scratch)
        write (*,*) '    inputimage  outputimage'
        call operateimage(f1,f2)

        subroutine callsu(scratch)
        write (*,*) '    inputimage  outputimage  tv/wr'
        call subim(f1,f2,code)

        subroutine callis(scratch)
        write (*,*) '    image  k-sigma'
        call imagestats(f1,kappa,scratch)

        subroutine callsc(scratch)
        write (*,*) '    inputimage  outputimage  n(-sigma cut)'
        call nsigmacut(f1,f2,nsig)

        subroutine callpb(scratch)
        write (*,*) '    inputimage  outputimage'
        call primarybeam(f1,f2)

        subroutine callrh(scratch)
        write (*,*) '    inputimage'
        call readheader(f1,scratch,extn)

        subroutine callch(scratch)
        write (*,*) '    image_copyfrom  image_copyto'
        call copyheader(f1,f2,scratch,extn)

        subroutine callph(scratch)
        write (*,*) '    imagename'
        call puthead(f1,scratch,extn)

        subroutine callsl(scratch)
        write (*,*) '    imagename'
        call slice(f1)

        subroutine callrb(scratch,runcode)
        write (*,*) '    inputfile opfile '
        call callremag(f1,f2,scratch,runcode)

        subroutine callft(scratch,runcode)
        write (*,*) '    inputfile'
        call callfft(imagename,scratch,runcode)

        subroutine callgc(scratch,srldir,runcode,fitsdir)
        write (*,*) '    FITSfilename '
        call call_getcoords(fitsdir,scratch,fitsname,runcode)

        subroutine callrf(fitsdir,scratch,runcode)
        write (*,*) '    FITSfilename '
        call callreadfits(fitsdir,fitsname,filename,filename,
     /       scratch,runcode)

        
        data options/'cn','ri','cg','cm','mi','mf','mn','dt','qn',
     /               'di','ac','li','df','ps','ci','is','sc','pb',
     /               'rh','ch','cc','sf','ph','pp','fg','sl','su',
     /               'io','mo','tr','ex','ff','dd','la','nb','sm',
     /               'gp','rf','cs','cl','ta','rb','gc','ft','sh',
     /               'ai','at','fr','ms','fs','co'/


