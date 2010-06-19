
        subroutine callreadfits(fitsname,filename,
     /             f2,fitsdir,scratch,runcode)
        implicit none
        integer status,unit,readwrite,blocksize,naxes(4),nfound
        integer i,nchar,ndim
        character fitsname*500,f1*500,fitsdir*500,scratch*500,runcode*2
        character filename*500,f2*500,cmd*500

cf2py   intent(in) fitsdir,fitsname,filename,f2,scratch,runcode

        write (*,*) '  Reading ',fitsdir(1:nchar(fitsdir))//
     /              fitsname(1:nchar(fitsname))
        status=0
        call ftgiou(unit,status)
        readwrite=0
        f1=fitsdir(1:nchar(fitsdir))//fitsname(1:nchar(fitsname))
        call ftopen(unit,f1,readwrite,blocksize,status)

        call ftgknj(unit,'NAXIS',1,4,naxes,nfound,status)
        if (nfound.lt.1.or.nfound.gt.4) then
         write (*,*) 'READIMAGE failed to read the NAXISn keywords.'
         return
        end if
        write (*,'(a25,i6,$)') '   Size of FITS image is ',naxes(1)
        do i=2,nfound
         write (*,'(a3,i6,$)') ' X ',naxes(i)
        end do
        write (*,*) 

        call ftclos(unit, status)
        call ftfiou(unit, status)
        if (status .gt. 0)call printerror(status)

        if (nfound.eq.2.or.(nfound.gt.2.and.naxes(3).eq.1)) ndim=2
        if (nfound.gt.2.and.naxes(3).gt.1) ndim=3
        if (ndim.eq.2) call readfits2d(fitsdir,fitsname,
     /      naxes(1),naxes(2),runcode,filename,f2)
        if (ndim.eq.3) call readfits3d(fitsdir,fitsname,
     /      naxes(1),naxes(2),naxes(3),runcode,filename,f2)
        call readfitshead(fitsdir,fitsname,f2,scratch)
        call check_beaminheader(scratch,f2)
        if (f2(1:nchar(f2)).ne.filename(1:nchar(filename))) then
         cmd="cp "//scratch(1:nchar(scratch))//f2(1:nchar(f2))//
     /      ".header "//scratch(1:nchar(scratch))//
     /      filename(1:nchar(filename))//".header"
         call system(cmd)
        end if

        return
        end
c!
c!
c!
        subroutine readfits2d(fitsdir,fitsname,n,m,runcode,filename,
     /             fullname)
        implicit none
        integer status,unit,readwrite,blocksize,naxes(4),nfound
        integer group,i,nchar,n,m,j
        real*8 nullval
        real*8 image(n,m)
        logical anynull
        character fitsname*500,f1*500,fitsdir*500,runcode*2,filename*500
        character fullname*500

        status=0
        call ftgiou(unit,status)
        readwrite=0
        f1=fitsdir(1:nchar(fitsdir))//fitsname(1:nchar(fitsname))
        call ftopen(unit,f1,readwrite,blocksize,status)

        call ftgknj(unit,'NAXIS',1,2,naxes,nfound,status)
        group=1
        nullval=-999.d0
        call ftg2dd(unit,group,nullval,naxes(1),naxes(1),naxes(2),
     /       image,anynull,status)
        call ftclos(unit, status)
        call ftfiou(unit, status)
        if (status .gt. 0)call printerror(status)

        call writearray_bin2D(image,n,m,n,m,filename,runcode)

        return
        end
c!
c!
c!
        subroutine readfits3d(fitsdir,fitsname,n,m,l,runcode,filename,
     /             fullname)
        implicit none
        integer status,unit,readwrite,blocksize,naxes(4),nfound
        integer group,i,nchar,n,m,j,l,k
        real*8 nullval
        real*8 image(n,m,l)
        logical anynull
        character fitsname*500,f1*500,fitsdir*500,runcode*2,filename*500
        character fullname*500

        status=0
        call ftgiou(unit,status)
        readwrite=0
        f1=fitsdir(1:nchar(fitsdir))//fitsname(1:nchar(fitsname))
        call ftopen(unit,f1,readwrite,blocksize,status)

        call ftgknj(unit,'NAXIS',1,3,naxes,nfound,status)
        group=1
        nullval=-999.d0
        call ftg3dd(unit,group,nullval,naxes(1),naxes(2),naxes(1),
     /       naxes(2),naxes(3),image,anynull,status)
        call ftclos(unit, status)
        call ftfiou(unit, status)
        if (status .gt. 0) call printerror(status)

        call writearray_bin3D(image,n,m,l,filename,runcode)

        return
        end



