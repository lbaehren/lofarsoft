c! write coef as bin table. para as keywords

        subroutine writefitsshapcoef(fullname,fitsdir,fitsd,nmax,cf,
     /        beta,xc,yc,delx,dely)
        implicit none
        integer nmax,delx,dely,nchar,i,bnchar,j
        character fitsdir*500,fitsd*500,srldir*500,fullname*500
        character filename*500,str*10,extname*16
        real*8 xc,yc,beta,cf(nmax,nmax),cfvec(nmax),keyval
        logical simple,extend
        integer status,unit,readwrite,hdutype,nrows,bitpix
        integer naxis,naxes(3),blocksize,varidat,frow,felem
        character*16 ttype(nmax),tform(nmax),tunit(nmax)
        character keyword*16,comment*48

        nrows=nmax
        do i=1,nmax
         call int2str(i,str)
         ttype(i)='i='//str(bnchar(str):nchar(str))
         tform(i)='1D'
         tunit(i)=' '
        end do
        extname='BDSM_shapecoefs'

c! first write empty image. close it. then write BINTABLE.
        status=0
        filename=fitsd(1:nchar(fitsd))//fullname(1:nchar(fullname))//
     /           '.shapcoef.FITS'
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

        call ftclos(unit, status)
        call ftfiou(unit, status)
        if (status .gt. 0)call printerror(status)

c! open again
        status=0
        call ftgiou(unit,status)
        readwrite=1
        call ftopen(unit,filename,readwrite,blocksize,status)
        call ftmahd(unit,1,hdutype,status)
        call ftcrhd(unit,status)

        varidat=0
        call ftphbn(unit,nrows,nmax,ttype,tform,tunit,
     /            extname,varidat,status)
        call ftpkyd(unit,'BETA',beta,5,
     /       'Scale parameter for shapelet',status)
        call ftpkyj(unit,'NMAX',nmax,
     /       'Maximum order, size of matrix',status)
        call ftpkyd(unit,'XCENTRE',xc,9,
     /       'X coord of centre in image',status)
        call ftpkyd(unit,'YCENTRE',yc,9,
     /       'Y coord of centre in image',status)
        call ftpkyj(unit,'DELTA_X',delx,
     /       'DELTA_X + x_shapelet = x_image',status)
        call ftpkyj(unit,'DELTA_Y',dely,
     /       'DELTA_Y + y_shapelet = y_image',status)

        frow=1
        felem=1

        do i=1,nmax
         do j=1,nmax
          cfvec(j)=cf(i,j)
         end do
         call ftpcld(unit,i,frow,felem,nrows,cfvec,status)  
        end do

        call ftclos(unit, status)
        call ftfiou(unit, status)
        if (status .gt. 0)call printerror(status)
        
        return
        end

