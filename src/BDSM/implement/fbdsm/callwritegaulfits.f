c! write .gaul as binary FITS table (for AWE).
c! have to edit this file for CHANGE GAUL FORMAT.
c! HARDCODED 44 columns !!!

        subroutine callwritegaulfits(srldir,fitsname,f2,
     /             imagename,extname,fitsdir,version)
        implicit none
        character srldir*500,fitsname*500,extname*16
        integer n,m,nsrc,nffmt,nisl,nchar,gpi
        character f1*500,f2*500,ffmt*500,writeout*500
        character fitsdir*500,imagename*500,version*68
        
        write (*,*) '  Writing FITS gaussian catalogue '
        call sourcelistheaders(f2,f1,n,m,nisl,nsrc,gpi,nffmt,
     /       ffmt,srldir)
        call writegaulfits(f2,nsrc,44,srldir,fitsname,extname,
     /       fitsdir,version)

        return
        end


        subroutine writegaulfits(f2,nrows,tfields,srldir,fitsname,
     /             extname,fitsdir,version)
        implicit none
        integer status,unit,readwrite,hdutype,tfields,nrows,bitpix
        integer varidat,colnum,frow,felem,blocksize,i
        integer naxis,naxes(3)  
        logical simple,extend
        character srldir*500,filename*500,extname*16,f2*500,fitsname*500
        character*16 ttype(44),tform(44),tunit(44),version*68
        real*8 totfl(nrows),etotfl(nrows),peakfl(nrows),epeakfl(nrows)
        real*8 ss(nrows),sa(nrows),ess(nrows),esa(nrows),xpix(nrows)
        real*8 expix(nrows),ypix(nrows),eypix(nrows),bmaj(nrows)
        real*8 ebmaj(nrows),bmin(nrows),ebmin(nrows),bpa(nrows)
        real*8 ebpa(nrows),dbmaj(nrows),edbmaj(nrows),dbmin(nrows)
        real*8 edbmin(nrows),dbpa(nrows),edbpa(nrows),rmssrc(nrows)
        real*8 avsrc(nrows),rmsisl(nrows),avisl(nrows),chisq(nrows)
        real*8 q(nrows),ra(nrows),dec(nrows),era(nrows),edec(nrows)
        real*8 dumr1(nrows),dumr2(nrows),dumr3(nrows),dumr4(nrows)
        real*8 dumr5(nrows),dumr6(nrows)
        integer iisl(nrows),isrc(nrows),flag(nrows),nchar,srcnum(nrows)
        integer blc1(nrows),blc2(nrows),trc1(nrows),trc2(nrows)
        integer funit,nkeys,nspace
        character record*80,fitsdir*500,f1*500,head*30

        data ttype/'gaul_id','island_id','flag','Total_Flux',
     /    'Err_total_flux','Peak_flux','Err_peak_flux','RA','Err_RA',
     /    'DEC','Err_DEC','Xpos','Err_xpos','Ypos','Err_ypos','Bmaj_fw',
     /    'Err_bmaj','Bmin_fw','Err_bmin','Bpa','Err_bpa',
     /    'Deconv_bmaj_fw','Err_decon_bmaj','Deconv_bmin',
     /    'Err_decon_bmin','Deconv_bpa','Err_decon_bpa','src_rms',
     /    'src_av','isl_rms','isl_av','sp_in','e_sp_in','srcnum','blc1',
     /    'blc2','trc1','trc2','im_rms','dummy2','dummy3','dummy4',
     /    'dummy5','dummy6'/
        data tform/'1J','1J','1J','1D','1D','1D','1D','1D','1D','1D',
     /    '1D','1D','1D','1D','1D','1D','1D','1D',
     /    '1D','1D','1D','1D','1D','1D','1D','1D','1D','1D','1D','1D',
     /    '1D','1D','1D','1J','1J','1J','1J','1J','1D','1D','1D','1D',
     /    '1D','1D'/
        data tunit/' ',' ',' ','Jy','Jy','Jy','Jy','deg','deg','deg',
     /    'deg','pix','pix','pix','pix','arcsec','arcsec','arcsec',
     /    'arcsec','deg','deg','arcsec','arcsec','arcsec','arcsec',
     /    'deg','deg','Jy','Jy','Jy','Jy',' ',' ',' ',' ',' ',' ',' ',
     /    ' ',' ',' ',' ',' ',' '/

c! first write empty image. close it. then write BINTABLE.
        status=0
        filename=srldir(1:nchar(srldir))//f2(1:nchar(f2))//'.gaul.FITS'
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
     /        head(1:5).ne.'NAXIS'.and.head.ne.'EXTEND')
     /     call ftprec(unit,record,status)
         end if
        end do

        call ftclos(unit, status)
        call ftfiou(unit, status)
        call ftclos(funit, status)
        call ftfiou(funit, status)
        if (status .gt. 0)call printerror(status)
c!
c! read binary .gaul first
c!
        open(unit=21,file=srldir(1:nchar(srldir))//f2(1:nchar(f2))//
     /       '.gaul.bin',form='unformatted')
        do i=1,nrows
         read (21) iisl(i),isrc(i),flag(i),totfl(i),etotfl(i),peakfl(i)
     /    ,epeakfl(i),ra(i),era(i),dec(i),edec(i),
     /    xpix(i),expix(i),ypix(i),eypix(i),bmaj(i),ebmaj(i),bmin(i),
     /    ebmin(i),bpa(i),ebpa(i),dbmaj(i),edbmaj(i),dbmin(i),edbmin(i)
     /    ,dbpa(i),edbpa(i),rmssrc(i),avsrc(i),rmsisl(i),avisl(i),
     /    chisq(i),q(i),srcnum(i),blc1(i),blc2(i),trc1(i),trc2(i),
     /    dumr1(i),dumr2(i),dumr3(i),dumr4(i),dumr5(i),dumr6(i)
        end do
        close(21)

c!
c! Now open it again and write the BINTABLE extension.
c!
        status=0
        filename=srldir(1:nchar(srldir))//f2(1:nchar(f2))//'.gaul.FITS'
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

        call ftpclj(unit,1,frow,felem,nrows,iisl,status)  
        call ftpclj(unit,2,frow,felem,nrows,isrc,status)  
        call ftpclj(unit,3,frow,felem,nrows,flag,status)  
        call ftpcld(unit,4,frow,felem,nrows,totfl,status)  
        call ftpcld(unit,5,frow,felem,nrows,etotfl,status)  
        call ftpcld(unit,6,frow,felem,nrows,peakfl,status)  
        call ftpcld(unit,7,frow,felem,nrows,epeakfl,status)  
        call ftpcld(unit,8,frow,felem,nrows,ra,status) 
        call ftpcld(unit,9,frow,felem,nrows,era,status)
        call ftpcld(unit,10,frow,felem,nrows,dec,status)
        call ftpcld(unit,11,frow,felem,nrows,edec,status)
        call ftpcld(unit,12,frow,felem,nrows,xpix,status)
        call ftpcld(unit,13,frow,felem,nrows,expix,status)
        call ftpcld(unit,14,frow,felem,nrows,ypix,status)
        call ftpcld(unit,15,frow,felem,nrows,eypix,status)
        call ftpcld(unit,16,frow,felem,nrows,bmaj,status)
        call ftpcld(unit,17,frow,felem,nrows,ebmaj,status)
        call ftpcld(unit,18,frow,felem,nrows,bmin,status)
        call ftpcld(unit,19,frow,felem,nrows,ebmin,status)
        call ftpcld(unit,20,frow,felem,nrows,bpa,status)
        call ftpcld(unit,21,frow,felem,nrows,ebpa,status)
        call ftpcld(unit,22,frow,felem,nrows,dbmaj,status)
        call ftpcld(unit,23,frow,felem,nrows,edbmaj,status)
        call ftpcld(unit,24,frow,felem,nrows,dbmin,status)
        call ftpcld(unit,25,frow,felem,nrows,edbmin,status)
        call ftpcld(unit,26,frow,felem,nrows,dbpa,status)
        call ftpcld(unit,27,frow,felem,nrows,edbpa,status)
        call ftpcld(unit,28,frow,felem,nrows,rmssrc,status)
        call ftpcld(unit,29,frow,felem,nrows,avsrc,status)
        call ftpcld(unit,30,frow,felem,nrows,rmsisl,status)
        call ftpcld(unit,31,frow,felem,nrows,avisl,status)
        call ftpcld(unit,32,frow,felem,nrows,chisq,status)
        call ftpcld(unit,33,frow,felem,nrows,q,status)
        call ftpclj(unit,34,frow,felem,nrows,srcnum,status)  
        call ftpclj(unit,35,frow,felem,nrows,blc1,status)  
        call ftpclj(unit,36,frow,felem,nrows,blc2,status)  
        call ftpclj(unit,37,frow,felem,nrows,trc1,status)  
        call ftpclj(unit,38,frow,felem,nrows,trc2,status)  
        call ftpcld(unit,39,frow,felem,nrows,dumr1,status)
        call ftpcld(unit,40,frow,felem,nrows,dumr2,status)
        call ftpcld(unit,41,frow,felem,nrows,dumr3,status)
        call ftpcld(unit,42,frow,felem,nrows,dumr4,status)
        call ftpcld(unit,43,frow,felem,nrows,dumr5,status)
        call ftpcld(unit,44,frow,felem,nrows,dumr6,status)
  
        call ftclos(unit, status)
        call ftfiou(unit, status)
        if (status .gt. 0)call printerror(status)


        end

