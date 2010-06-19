c! reads just the astrometry keywords from a 2d fits file.
c! set status=0 before closing cos status ne 0 becos fo lack of keywords
c! screws up the closing.
        
        subroutine read_astrometry2(filen,dir,n,m,ctype,crval,
     /             crpix,cdelt,crota)
        implicit none
        character filen*500,ctype(2)*8,dir*500,fn*500,dumc8*8,dumc*1
        character record*80
        integer n,m,status,readwrite,funit,blocksize,nchar
        real*8 blcc,trcc,crpix(2),cdelt(2),crval(2),crota(2)

        fn=dir(1:nchar(dir))//filen(1:nchar(filen))
        status=0
        call ftgiou(funit,status)
        readwrite=0
        call ftopen(funit,fn,readwrite,blocksize,status)

        call ftgcrd(funit,'NAXIS1',record,status)
        read (record,*) dumc8,dumc,n
        call ftgcrd(funit,'NAXIS2',record,status)
        read (record,*) dumc8,dumc,m
        call ftgcrd(funit,'CTYPE1',record,status)
        read (record,*) dumc8,dumc,ctype(1)
        call ftgcrd(funit,'CTYPE2',record,status)
        read (record,*) dumc8,dumc,ctype(2)
        call ftgcrd(funit,'CRPIX1',record,status)
        read (record,*) dumc8,dumc,crpix(1)
        call ftgcrd(funit,'CRPIX2',record,status)
        read (record,*) dumc8,dumc,crpix(2)
        call ftgcrd(funit,'CRVAL1',record,status)
        read (record,*) dumc8,dumc,crval(1)
        call ftgcrd(funit,'CRVAL2',record,status)
        read (record,*) dumc8,dumc,crval(2)
        call ftgcrd(funit,'CDELT1',record,status)
        read (record,*) dumc8,dumc,cdelt(1)
        call ftgcrd(funit,'CDELT2',record,status)
        read (record,*) dumc8,dumc,cdelt(2)
        status=0
        call ftgcrd(funit,'CROTA1',record,status)
        read (record,*) dumc8,dumc,crota(1)
        if (status.gt.0) crota(1)=0.d0
        status=0
        call ftgcrd(funit,'CROTA2',record,status)
        read (record,*) dumc8,dumc,crota(2)
        if (status.gt.0) crota(2)=0.d0
        status=0
        
        call ftclos(funit, status)
        call ftfiou(-1, status)

        return
        end



