
      subroutine callreadfits(fitsdir,fitsname,filename,scratch,runcode)
      implicit none
      integer status,unit,readwrite,blocksize,naxes(4),nfound
      integer i,nchar
      character fitsname*500,f1*500,fitsdir*500,scratch*500,runcode*2
      character filename*500

      write (*,*) '  Reading ',fitsdir(1:nchar(fitsdir))//
     /            fitsname(1:nchar(fitsname))
      status=0
      call ftgiou(unit,status)
      readwrite=0
      f1=fitsdir(1:nchar(fitsdir))//fitsname(1:nchar(fitsname))
      call ftopen(unit,f1,readwrite,blocksize,status)

      call ftgknj(unit,'NAXIS',1,2,naxes,nfound,status)
c      if (nfound.ne.4)then
c       write (*,*) 'READIMAGE failed to read the NAXISn keywords.'
c       return
c      end if
      write (*,'(a25,i6,a3,i6)') 
     /      '   Size of FITS image is ',naxes(1),' X ',naxes(2)

      call ftclos(unit, status)
      call ftfiou(unit, status)
      if (status .gt. 0)call printerror(status)

      call readfits(fitsdir,fitsname,naxes(1),naxes(2),runcode,filename)
      call readfitshead(fitsdir,fitsname,scratch,filename)
      return
      end

      subroutine readfits(fitsdir,fitsname,n,m,runcode,filename)
      implicit none
      integer status,unit,readwrite,blocksize,naxes(4),nfound
      integer group,i,nchar,n,m
      real nullval,buff(n*m)
      real*8 image(n,m)
      logical anynull
      character fitsname*500,f1*500,fitsdir*500,runcode*2,filename*500

      status=0
      call ftgiou(unit,status)
      readwrite=0
      f1=fitsdir(1:nchar(fitsdir))//fitsname(1:nchar(fitsname))
      call ftopen(unit,f1,readwrite,blocksize,status)

      call ftgknj(unit,'NAXIS',1,2,naxes,nfound,status)
c      if (nfound.ne.4)then
c       write (*,*) 'READIMAGE failed to read the NAXISn keywords.'
c       return
c      end if
      group=1
      nullval=-999

      call ftgpve(unit,group,1,n*m,nullval,buff,anynull,status)
      do i=1,n*m
       image(i-((i-1)/n)*n,(i-1)/n+1)=buff(i)
      end do

      call ftclos(unit, status)
      call ftfiou(unit, status)

      if (status .gt. 0)call printerror(status)

      call writearray_bin(image,n,m,n,m,filename,runcode)

      return
      end


