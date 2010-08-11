c! write .shapelet.c as binary FITS table (for AWE).
c! HARDCODED 11 columns !!!

        subroutine callwriteshapfits(srldir,fitsname,f2,
     /             imagename,extname,fitsdir,version)
        implicit none
        character srldir*500,fitsname*500,extname*16
        integer n,m,nsrc,nffmt,nisl,nchar,gpi
        character f1*500,f2*500,ffmt*500,writeout*500
        character fitsdir*500,imagename*500,version*68
        
        write (*,*) '  Writing shapelet parameter FITS catalogue '
        call sourcelistheaders(f2,f1,n,m,nisl,nsrc,gpi,nffmt,
     /       ffmt,srldir)
        call writeshapfits(f2,nisl,11,srldir,fitsname,extname,
     /       fitsdir,version)

        return
        end


        subroutine writeshapfits(f2,nrows,tfields,srldir,fitsname,
     /             extname,fitsdir,version)
        implicit none
        integer status,unit,readwrite,hdutype,tfields,nrows,bitpix
        integer varidat,colnum,frow,felem,blocksize,i
        integer naxis,naxes(3),nchar
        logical simple,extend
        character srldir*500,filename*500,extname*16,f2*500,fitsname*500
        character*16 ttype(11),tform(11),tunit(11),version*68
        real*8 xcen(nrows),ycen(nrows),beta(nrows),isl_std(nrows)
        real*8 isl_av(nrows)
        integer nmax(nrows),iisl(nrows)
        integer blc1(nrows),blc2(nrows),trc1(nrows),trc2(nrows)
        integer funit,nkeys,nspace
        character record*80,fitsdir*500,f1*500,head*30,dumc*500

        data ttype/'island_id','nmax','beta','xcentre',
     /    'ycentre','std_res','av_res','blc1','blc2',
     /    'trc1','trc2'/
        data tform/'1J','1J','1D','1D','1D','1D','1D','1J','1J',
     /    '1J','1J'/
        data tunit/' ',' ','pix','pix','pix','Jy','Jy',' ',' ',' ',
     /    ' '/

c! first write empty image. close it. then write BINTABLE.
        status=0
        filename=srldir(1:nchar(srldir))//f2(1:nchar(f2))//
     /           '.shapelet.c.FITS'
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
         read (record,*) head
         if (head.ne.'SIMPLE'.and.head.ne.'BITPIX'.and.
     /       head(1:5).ne.'NAXIS'.and.head.ne.'EXTEND')
     /    call ftprec(unit,record,status)
        end do

        call ftclos(unit, status)
        call ftfiou(unit, status)
        call ftclos(funit, status)
        call ftfiou(funit, status)
        if (status .gt. 0)call printerror(status)
c!
c! read shap file
c!
        open(unit=21,file=srldir(1:nchar(srldir))//f2(1:nchar(f2))//
     /       '.shapelet.c',form='formatted')
333     read (21,*) dumc
        if (dumc(1:3).ne.'fmt') goto 333
        do i=1,nrows
         read (21,*) iisl(i),nmax(i),beta(i),xcen(i),ycen(i),
     /    isl_std(i),isl_av(i),blc1(i),blc2(i),trc1(i),
     /    trc2(i)
        end do
        close(21)

c!
c! Now open it again and write the BINTABLE extension.
c!
        status=0
        filename=srldir(1:nchar(srldir))//f2(1:nchar(f2))//
     /           '.shapelet.c.FITS'
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
        call ftpclj(unit,2,frow,felem,nrows,nmax,status)  
        call ftpcld(unit,3,frow,felem,nrows,beta,status)  
        call ftpcld(unit,4,frow,felem,nrows,xcen,status)  
        call ftpcld(unit,5,frow,felem,nrows,ycen,status)  
        call ftpcld(unit,6,frow,felem,nrows,isl_std,status)  
        call ftpcld(unit,7,frow,felem,nrows,isl_av,status)  
        call ftpclj(unit,8,frow,felem,nrows,blc1,status) 
        call ftpclj(unit,9,frow,felem,nrows,blc2,status)
        call ftpclj(unit,10,frow,felem,nrows,trc1,status)
        call ftpclj(unit,11,frow,felem,nrows,trc2,status)
  
        call ftclos(unit, status)
        call ftfiou(unit, status)
        if (status .gt. 0)call printerror(status)


        end

