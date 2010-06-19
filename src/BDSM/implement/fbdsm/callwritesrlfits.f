c! write .srl as binary FITS table (for AWE).
c! have to edit this file for CHANGE SRL FORMAT.
c! HARDCODE number of columns 36

        subroutine callwritesrlfits(srldir,fitsname,f2,
     /             imagename,extname,fitsdir,version)
        implicit none
        character srldir*500,fitsname*500,extname*16,fn*500
        integer n,m,nsrc,nffmt,nisl,nchar,gpi
        character f1*500,f2*500,ffmt*500,dumc*500
        character fitsdir*500,imagename*500,version*68
        real*8 dumr
        
        write (*,*) '  Writing FITS source catalogue '
        fn=srldir(1:nchar(srldir))//f2(1:nchar(f2))//'.srl'   
        open(unit=22,file=fn(1:nchar(fn)),form='formatted') 
        call readhead_srclist(22,17,'Number_of_sources',dumc,nsrc,
     /       dumr,'i')
        close(22)
        call writesrlfits(f2,nsrc,36,srldir,fitsname,extname,fitsdir,
     /       version)

        return
        end


        subroutine writesrlfits(f2,nrows,tfields,srldir,fitsname,
     /             extname,fitsdir,version)
        implicit none
        integer status,unit,readwrite,hdutype,tfields,nrows,bitpix
        integer varidat,colnum,frow,felem,blocksize,i
        integer naxis,naxes(3)  
        logical simple,extend
        character srldir*500,filename*500,extname*16,f2*500,fitsname*500
        character ttype(36)*16,tform(36)*16,tunit(36)*16,code(nrows)*4
        integer nchar,numsrc(nrows),sflag(nrows),ngau(nrows)
        integer blc1(nrows),blc2(nrows),trc1(nrows),trc2(nrows)
        real*8 total(nrows),etotfl(nrows),speak(nrows),epeakfl(nrows)
        real*8 max_peak(nrows),sra(nrows),era(nrows)
        real*8 sdec(nrows),edec(nrows),mra(nrows),mdec(nrows)
        real*8 bmaj(nrows),ebmaj(nrows),bmin(nrows)
        real*8 ebmin(nrows),bpa(nrows),ebpa(nrows),dbmaj(nrows)
        real*8 edbmaj(nrows),dbmin(nrows),edbmin(nrows),dbpa(nrows)
        real*8 edbpa(nrows),rmsisl(nrows)
        integer funit,nkeys,nspace,iisl(nrows)
        character record*80,fitsdir*500,f1*500,head*30,version*68

        data ttype/'src_id','isl#','flag','Code','Total_Flux',
     /    'Err_total_flux','Cen_Peak_flux','Err_Cpeak_flux',
     /    'Max_Peak_Flux','Err_Mpeak_flux',
     /    'Cen_RA','Err_CRA','Cen_DEC','Err_CDEC',
     /    'Max_RA','Err_MRA','Max_DEC','Err_MDEC',
     /    'Bmaj_fw','Err_bmaj','Bmin_fw','Err_bmin','Bpa','Err_bpa',
     /    'Deconv_bmaj_fw','Err_decon_bmaj','Deconv_bmin',
     /    'Err_decon_bmin','Deconv_bpa','Err_decon_bpa',
     /    'isl_rms','num_gau','blc1','blc2','trc1','trc2'/
        data tform/'1J','1J','1J','1A','1D','1D','1D','1D','1D',
     /    '1D','1D',
     /    '1D','1D','1D','1D','1D','1D','1D','1D','1D','1D','1D','1D',
     /    '1D','1D','1D','1D','1D','1D','1D','1D','1J','1J','1J','1J',
     /    '1J'/
        data tunit/' ',' ',' ',' ','Jy','Jy','Jy','Jy','Jy','Jy',
     /    'deg','deg','deg','deg','deg','deg','deg','deg',
     /    'arcsec','arcsec','arcsec',
     /    'arcsec','deg','deg','arcsec','arcsec','arcsec','arcsec',
     /    'deg','deg','Jy',' ',' ',' ',' ',' '/

c! first write empty image. close it. then write BINTABLE.
        status=0
        filename=srldir(1:nchar(srldir))//f2(1:nchar(f2))//'.srl.FITS'
        call deletefile(filename,status)
        call ftgiou(unit,status)
        blocksize=1
        call ftinit(unit,filename,blocksize,status)
        simple=.true.
        bitpix=16
        naxis=0
        naxes(1)=0
        naxes(2)=0
        naxes(3)=0
        extend=.true.
        call ftphpr(unit,simple,bitpix,naxis,naxes,0,1,extend,status)

c! try write astrometry keywords in header, for AWE
        f1=fitsdir(1:nchar(fitsdir))//
     /      fitsname(1:nchar(fitsname))
        status=0
        call ftgiou(funit,status)
        readwrite=0
        call ftopen(funit,f1,readwrite,blocksize,status)
        call ftghsp(funit,nkeys,nspace,status)  ! nkeys keywords present
        do i = 1, nkeys
         call ftgrec(funit,i,record,status)
         if (record.ne.'') then
          read (record,*) head
          if (head.ne.'SIMPLE'.and.head.ne.'BITPIX'.and.
     /        head(1:5).ne.'NAXIS'.and.head.ne.'EXTEND') then
           call ftprec(unit,record,status)
          end if
         end if
        end do

        call ftclos(unit, status)
        call ftfiou(unit, status)
        call ftclos(funit, status)
        call ftfiou(funit, status)
        if (status .gt. 0)call printerror(status)
c!
c! read binary .srl first
c!
        open(unit=21,file=srldir(1:nchar(srldir))//f2(1:nchar(f2))//
     /       '.srl.bin',form='unformatted')
        do i=1,nrows
         read (21) numsrc(i),iisl(i),sflag(i),code(i),total(i),
     /     etotfl(i),
     /     speak(i),epeakfl(i),max_peak(i),epeakfl(i),sra(i),era(i),
     /     sdec(i),edec(i),mra(i),era(i),mdec(i),edec(i),
     /     bmaj(i),ebmaj(i),bmin(i),ebmin(i),bpa(i),ebpa(i),
     /     dbmaj(i),edbmaj(i),dbmin(i),edbmin(i),dbpa(i),edbpa(i),
     /     rmsisl(i),ngau(i),blc1(i),blc2(i),trc1(i),trc2(i)
        end do
        close(21)
c!
c! Now open it again and write the BINTABLE extension.
c!
        status=0
        filename=srldir(1:nchar(srldir))//f2(1:nchar(f2))//'.srl.FITS'
        call ftgiou(unit,status)
        readwrite=1
        call ftopen(unit,filename,readwrite,blocksize,status)
        call ftmahd(unit,1,hdutype,status)
        call ftcrhd(unit,status)
        varidat=0
        call ftphbn(unit,nrows,tfields,ttype,tform,tunit,
     /            extname,varidat,status)
        call ftpkys(unit,'VERSION',version(1:nchar(version)),
     /       'Version of BDSM',status)
       
        frow=1
        felem=1

        call ftpclj(unit,1,frow,felem,nrows,numsrc,status)  
        call ftpclj(unit,2,frow,felem,nrows,iisl,status)  
        call ftpclj(unit,3,frow,felem,nrows,sflag,status)  
        call ftpcls(unit,4,frow,felem,nrows,code,status)  
        call ftpcld(unit,5,frow,felem,nrows,total,status)  
        call ftpcld(unit,6,frow,felem,nrows,etotfl,status)  
        call ftpcld(unit,7,frow,felem,nrows,speak,status)  
        call ftpcld(unit,8,frow,felem,nrows,epeakfl,status)  
        call ftpcld(unit,9,frow,felem,nrows,max_peak,status)  
        call ftpcld(unit,10,frow,felem,nrows,epeakfl,status)  
        call ftpcld(unit,11,frow,felem,nrows,sra,status) 
        call ftpcld(unit,12,frow,felem,nrows,era,status)
        call ftpcld(unit,13,frow,felem,nrows,sdec,status)
        call ftpcld(unit,14,frow,felem,nrows,edec,status)
        call ftpcld(unit,15,frow,felem,nrows,mra,status) 
        call ftpcld(unit,16,frow,felem,nrows,era,status)
        call ftpcld(unit,17,frow,felem,nrows,mdec,status)
        call ftpcld(unit,18,frow,felem,nrows,edec,status)
        call ftpcld(unit,19,frow,felem,nrows,bmaj,status)
        call ftpcld(unit,20,frow,felem,nrows,ebmaj,status)
        call ftpcld(unit,21,frow,felem,nrows,bmin,status)
        call ftpcld(unit,22,frow,felem,nrows,ebmin,status)
        call ftpcld(unit,23,frow,felem,nrows,bpa,status)
        call ftpcld(unit,24,frow,felem,nrows,ebpa,status)
        call ftpcld(unit,25,frow,felem,nrows,dbmaj,status)
        call ftpcld(unit,26,frow,felem,nrows,edbmaj,status)
        call ftpcld(unit,27,frow,felem,nrows,dbmin,status)
        call ftpcld(unit,28,frow,felem,nrows,edbmin,status)
        call ftpcld(unit,29,frow,felem,nrows,dbpa,status)
        call ftpcld(unit,30,frow,felem,nrows,edbpa,status)
        call ftpcld(unit,31,frow,felem,nrows,rmsisl,status)
        call ftpclj(unit,32,frow,felem,nrows,ngau,status)
        call ftpclj(unit,33,frow,felem,nrows,blc1,status)
        call ftpclj(unit,34,frow,felem,nrows,blc2,status)
        call ftpclj(unit,35,frow,felem,nrows,trc1,status)
        call ftpclj(unit,36,frow,felem,nrows,trc2,status)

c!
        call ftclos(unit, status)
        call ftfiou(unit, status)
        if (status .gt. 0)call printerror(status)


        end

