c! writes out 3d FITS file.

        subroutine writefits3D(im,ptsizex,ptsizey,nchan,telescop,
     /    filename,
     /    receiver,object,observer,epoch,fproduct,bmaj,bmin,bpa,ctype1,
     /    ctype2,ctype3,crval1,crval2,crval3,crpix1,crpix2,crpix3,
     /    cdelt1,cdelt2,cdelt3,fitsdir)
        implicit none
        integer status,unit,blocksize,bitpix,naxis,naxes(3)
        integer group,fpixel,nelements,nchar
        character filename*500
        logical simple,extend
        integer ptsizex,ptsizey,nchan
        real*8 im(ptsizex,ptsizey,nchan)
        real*8 epoch,bmaj,bmin,bpa,crval1,crval2,crval3
        real*8 crpix1,crpix2,crpix3,cdelt1,cdelt2,cdelt3
        character telescop*8,receiver*8,object*8,observer*8
        character ctype1*8,ctype2*8,ctype3*8,fitsdir*500
        integer fproduct
        real*4 var

        status=0
        filename=fitsdir(1:nchar(fitsdir))//
     /           filename(1:nchar(filename))//'.FITS'
        write (*,*) '  writing ',filename(1:nchar(filename))
        call deletefile(filename,status)
        call ftgiou(unit,status)
        blocksize=1
        call ftinit(unit,filename,blocksize,status)

        simple=.true.
        bitpix=-32
        naxis=3
        naxes(1)=ptsizex
        naxes(2)=ptsizey
        naxes(3)=nchan
        extend=.true.
  
        call ftphpr(unit,simple,bitpix,naxis,naxes,0,1,extend,status)

        group=1
        fpixel=1
        nelements=naxes(1)*naxes(2)*naxes(3)
        call ftpprd(unit,group,fpixel,nelements,im,status)
  
        call ftpkys(unit,'TELESCOP',telescop,'',status)
        call ftpkys(unit,'INSTRUME',receiver,'',status)
        call ftpkys(unit,'OBJECT',object,'',status)
        call ftpkys(unit,'OBSERVER',observer,'',status)
        call ftpkys(unit,'BUNIT','Jy/beam','units of image',status)
        call ftpkys(unit,'CTYPE1',ctype1,'',status)
        call ftpkys(unit,'CTYPE2',ctype2,'',status)
        call ftpkys(unit,'CTYPE3',ctype3,'',status)

        fproduct=1
        call ftpkyj(unit,'PRODUCT',fproduct,'',status)

        var=epoch
        call ftpkye(unit,'EPOCH',var,6,' ',status)
        var=bmaj
        call ftpkye(unit,'BMAJ',var,6,' ',status)
        var=bmin
        call ftpkye(unit,'BMIN',var,6,' ',status)
        var=bpa
        call ftpkye(unit,'BPA',var,2,' ',status)
  
        var=crpix1
        call ftpkye(unit,'CRPIX1',var,6,'',status)
        var=crpix2
        call ftpkye(unit,'CRPIX2',var,6,'',status)
        var=crpix3
        call ftpkye(unit,'CRPIX3',var,6,'',status)

        var=crval1
        call ftpkye(unit,'CRVAL1',var,8,'',status)
        var=crval2
        call ftpkye(unit,'CRVAL2',var,8,'',status)
        var=crval3
        call ftpkye(unit,'CRVAL3',var,8,'',status)

        var=cdelt1/3600.d0
        call ftpkye(unit,'CDELT1',var,8,'',status)
        var=cdelt2/3600.d0
        call ftpkye(unit,'CDELT2',var,8,'',status)
        var=cdelt3
        call ftpkye(unit,'CDELT3',var,8,'',status)

        call ftpkye(unit,'CROTA1',0.0,2,'',status)
        call ftpkye(unit,'CROTA2',0.0,2,'',status)
        call ftpkye(unit,'CROTA3',0.0,2,'',status)

        call ftclos(unit, status)
        call ftfiou(unit, status)
  
        if (status .gt. 0)call printerror(status)
        end



      subroutine printerror(status)
      integer status
      character errtext*30,errmessage*80

      if (status .le. 0)return
      call ftgerr(status,errtext)
      print *,'FITSIO Error Status =',status,': ',errtext
      call ftgmsg(errmessage)
      do while (errmessage .ne. ' ')
          print *,errmessage
          call ftgmsg(errmessage)
      end do
      return
      end

      subroutine deletefile(filename,status)
      integer status,unit,blocksize
      character*(*) filename

      if (status .gt. 0)return
      call ftgiou(unit,status)
      call ftopen(unit,filename,1,blocksize,status)
      if (status .eq. 0)then
          call ftdelt(unit,status)
      else if (status .eq. 103)then
          status=0
          call ftcmsg
      else
          status=0
          call ftcmsg
          call ftdelt(unit,status)
      end if
      call ftfiou(unit, status)
      return
      end
