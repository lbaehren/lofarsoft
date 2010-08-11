c! filen.img is the image to be written
c! modified to write 2d file as 4d etc -- for ilse.


        subroutine writefits2Das4D(image,x,y,n,m,filen,fitsdir,
     /             fitsfiledir,scratch,fitsname)
        implicit none
        integer status,unit,blocksize,bitpix,naxis,naxes(4)
        integer i,j,group,fpixel,nelements,x,y,n,m,nchar,error
        character filename*500,filen*500,fitsdir*500,keystrng*500
        character f1*500,record*80,fitsname*500,head*30,scratch*500
        character comment*500,keyword*500
        logical simple,extend
        real*8 image(x,y),array(n,m,1,1),keyvalue,bmaj,bmin,bpa
        integer funit,readwrite,nkeys,nspace,hdutype
        real*8 crpix(3),cdelt(3),crval(3),crota(3),bm_pix(3),nullval
        real*8 crpix4,cdelt4,crval4,crota4
        character ctype(3)*8,extn*20,fitsfiledir*500,ctype4*8

        ctype4=''
        status=0
        filename=fitsdir(1:nchar(fitsdir))//filen(1:nchar(filen))
     /           //'.FITS'
        call deletefile(filename,status)
        call ftgiou(unit,status)
        blocksize=1
        call ftinit(unit,filename,blocksize,status)

        simple=.true.
        bitpix=-32
        naxis=4
        naxes(1)=n
        naxes(2)=m
        naxes(3)=1
        naxes(4)=1
        extend=.true.
        nullval=-999.d0
  
        call ftphpr(unit,simple,bitpix,naxis,naxes,0,1,extend,status)

        do i=1,naxes(1)
         do j=1,naxes(2)
          array(i,j,1,1)=image(i,j)
         end do
        end do

        group=1
        fpixel=1
        nelements=naxes(1)*naxes(2)
        call ftppnd(unit,group,fpixel,nelements,array,nullval,status)
  
c!  -----
        if (fitsname.ne.' '.and.fitsname.ne.'header') then
         f1=fitsfiledir(1:nchar(fitsfiledir))//
     /      fitsname(1:nchar(fitsname))
         status=0
         call ftgiou(funit,status)
         call ftopen(funit,f1,readwrite,blocksize,status)
         call ftghsp(funit,nkeys,nspace,status)  ! nkeys keywords present
         do i = 1,nkeys
          call ftgrec(funit,i,record,status)
          read (record,*) head
          if (head.ne.'SIMPLE'.and.head.ne.'BITPIX'.and.
     /        head(1:5).ne.'NAXIS'.and.head.ne.'EXTEND')
     /     call ftprec(unit,record,status)
         end do
         call ftclos(funit, status)
         call ftfiou(funit, status)
        else
         if (fitsname.ne.'header') then
          call read_dum_head(3,ctype,crpix,cdelt,crval,crota,bm_pix)
          bmaj=bm_pix(1)*cdelt(1)
          bmin=bm_pix(2)*cdelt(2)
          bpa=bm_pix(3)
         else  ! read from .header file
          extn='.header'
          call readheader4fits(filen,extn,ctype,crpix,cdelt,crval,
     /         crota,bmaj,bmin,bpa,3,scratch)
          keyword='CTYPE4'
          call get_keyword(filen,extn,keyword,keystrng,keyvalue,
     /             comment,'s',scratch,error) 
          ctype4=keystrng(1:nchar(keystrng))
          keyword='CRPIX4'
          call get_keyword(filen,extn,keyword,keystrng,crpix4,
     /             comment,'r',scratch,error) 
          keyword='CRVAL4'
          call get_keyword(filen,extn,keyword,keystrng,crval4,
     /           comment,'r',scratch,error) 
          keyword='CDELT4'
          call get_keyword(filen,extn,keyword,keystrng,cdelt4,
     /             comment,'r',scratch,error) 
          keyword='CROTA4'
          call get_keyword(filen,extn,keyword,keystrng,crota4,
     /             comment,'r',scratch,error) 
         end if
         call ftpkyd(unit,'CRPIX1',crpix(1),6,' ',status)
         call ftpkyd(unit,'CRPIX2',crpix(2),6,' ',status)
         call ftpkyd(unit,'CRPIX3',crpix(3),6,' ',status)
         call ftpkyd(unit,'BMAJ',bmaj,6,' ',status)
         call ftpkyd(unit,'BMIN',bmin,6,' ',status)
         call ftpkyd(unit,'BPA',bpa,2,' ',status)
         call ftpkyd(unit,'CRVAL1',crval(1),8,' ',status)
         call ftpkyd(unit,'CRVAL2',crval(2),8,' ',status)
         call ftpkyd(unit,'CRVAL3',crval(3),8,' ',status)
         call ftpkyd(unit,'CDELT1',cdelt(1),8,' ',status)
         call ftpkyd(unit,'CDELT2',cdelt(2),8,' ',status)
         call ftpkyd(unit,'CDELT3',cdelt(3),8,' ',status)
         call ftpkyd(unit,'CROTA1',crota(1),2,' ',status)
         call ftpkyd(unit,'CROTA2',crota(2),2,' ',status)
         call ftpkyd(unit,'CROTA3',crota(3),2,' ',status)
         call ftpkys(unit,'CTYPE1',ctype(1),' ',status)
         call ftpkys(unit,'CTYPE2',ctype(2),' ',status)
         call ftpkys(unit,'CTYPE3',ctype(3),' ',status)
         if (ctype4.eq.'') then
          call ftpkyd(unit,'CRPIX4',1.d0,6,' ',status)
          call ftpkyd(unit,'CRVAL4',1.d0,8,' ',status)
          call ftpkyd(unit,'CDELT4',1.d0,8,' ',status)
          call ftpkyd(unit,'CROTA4',1.d0,2,' ',status)
          call ftpkys(unit,'CTYPE4','STOKES  ',' ',status)
         else
          call ftpkyd(unit,'CRPIX4',crpix4,6,' ',status)
          call ftpkyd(unit,'CRVAL4',crval4,8,' ',status)
          call ftpkyd(unit,'CDELT4',cdelt4,8,' ',status)
          call ftpkyd(unit,'CROTA4',crota4,2,' ',status)
          call ftpkys(unit,'CTYPE4',ctype4,' ',status)
         end if
        end if
c!  -----

        call ftclos(unit, status)
        call ftfiou(unit, status)
  
        if (status .gt. 0)call printerror(status)

        end

