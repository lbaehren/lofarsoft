
        subroutine writefits(image,x,y,n,m,filen,fitsdir,
     /             scratch,fitsname)
        implicit none
        integer status,unit,blocksize,bitpix,naxis,naxes(3)
        integer i,j,group,fpixel,nelements,x,y,n,m,nchar
        character filename*500,filen*500,fitsdir*500
        character f1*500,record*80,fitsname*500,head*30,scratch*500
        logical simple,extend
        real*8 image(x,y),array(n,m,1) 
        integer funit,readwrite,nkeys,nspace,hdutype
        real*8 crpix(3),cdelt(3),crval(3),crota(3),bm_pix(3)
        character ctype(3)*8

        status=0
        filename=fitsdir(1:nchar(fitsdir))//filen(1:nchar(filen))
        call deletefile(filename,status)
        call ftgiou(unit,status)
        blocksize=1
        call ftinit(unit,filename,blocksize,status)

        simple=.true.
        bitpix=-32
        naxis=3
        naxes(1)=n
        naxes(2)=m
        naxes(3)=1
        extend=.true.
  
        call ftphpr(unit,simple,bitpix,naxis,naxes,0,1,extend,status)

        do i=1,naxes(1)
         do j=1,naxes(2)
          array(i,j,1)=image(i,j)
         end do
        end do

        group=1
        fpixel=1
        nelements=naxes(1)*naxes(2)
        call ftpprd(unit,group,fpixel,nelements,array,status)
  
c!  -----
        if (fitsname.ne.' ') then
c          write (*,*) ' dont do this'
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
     /        head(1:5).ne.'NAXIS'.and.head.ne.'EXTEND')
     /     call ftprec(unit,record,status)
         end do
        else
         call read_dum_head(3,ctype,crpix,cdelt,crval,crota,bm_pix)
         call ftpkye(unit,'CRPIX1',crpix(1),6,' ',status)
         call ftpkye(unit,'CRPIX2',crpix(2),6,' ',status)
         call ftpkye(unit,'CRPIX3',crpix(3),6,' ',status)
         call ftpkys(unit,'CTYPE1',ctype(1),' ',status)
         call ftpkys(unit,'CTYPE2',ctype(2),' ',status)
         call ftpkys(unit,'CTYPE3',ctype(3),' ',status)
         call ftpkyd(unit,'BMAJ',bm_pix(1)*cdelt(1),6,' ',status)
         call ftpkyd(unit,'BMIN',bm_pix(2)*cdelt(2),6,' ',status)
         call ftpkyd(unit,'BPA',bm_pix(3),2,' ',status)
         call ftpkyd(unit,'CRVAL1',crval(1),8,' ',status)
         call ftpkyd(unit,'CRVAL2',crval(2),8,' ',status)
         call ftpkyd(unit,'CRVAL3',crval(3),8,' ',status)
         call ftpkyd(unit,'CDELT1',cdelt(1),8,' ',status)
         call ftpkyd(unit,'CDELT2',cdelt(2),8,' ',status)
         call ftpkyd(unit,'CDELT3',cdelt(3),8,' ',status)
         call ftpkyd(unit,'CROTA1',crota(1),2,' ',status)
         call ftpkyd(unit,'CROTA2',crota(2),2,' ',status)
         call ftpkyd(unit,'CROTA3',crota(3),2,' ',status)
        end if
c!  -----

        call ftclos(unit, status)
        call ftfiou(unit, status)
  
        if (status .gt. 0)call printerror(status)
        end

