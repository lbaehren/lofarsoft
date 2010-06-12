
      subroutine readfitshead(fitsdir,fitsname,scratch,filename)
      implicit none
      include "constants.inc" 
      integer status,funit,readwrite,blocksize,nkeys,nspace,hdutype,i
      integer nchar,dumi,err,dumi1
      character fitsname*500,record*80,fn*500,cmd*500,fn40*500
      character dumc*10,scratch*500,fitsdir*500,filename*500
      character fn2*500,extn*10,keystrng*500,comment*500,keyword*500
      real*8 bmaj,bmin,bpa,cdelt2,crval2,crpix2,dec,naxis2

      fn=fitsdir(1:nchar(fitsdir))//fitsname(1:nchar(fitsname))
      status=0
      call ftgiou(funit,status)
      readwrite=0
      call ftopen(funit,fn,readwrite,blocksize,status)
      fn=scratch(1:nchar(scratch))//
     /   filename(1:nchar(filename))//'.header'
      open(unit=funit+1,file=fn,status='unknown')

      call ftghsp(funit,nkeys,nspace,status)  ! nkeys keywords present

      do i = 1, nkeys
       call ftgrec(funit,i,record,status)
       write (funit+1,*) record
      end do

      call ftclos(funit, status)
      call ftfiou(funit, status)
      close(funit+1)

      if (status .gt. 0)call printerror(status)

      fn=scratch(1:nchar(scratch))//
     /   filename(1:nchar(filename))//'.header'
      call system('rm -f '//scratch(1:nchar(scratch))//'a')
      cmd="grep -v HISTORY "//fn(1:nchar(fn))//" > "//
     /    scratch(1:nchar(scratch))//"a"
      call system(cmd)

c! now to get the BMAJ etc out of the damn history file for AIPS images.
      call system("grep 'CLEAN BMAJ=' "//fn(1:nchar(fn))//
     /         " | wc -l > "//scratch(1:nchar(scratch))//"b")
      fn40='b' 
      call getline(fn40,scratch,dumi)
      if (dumi.eq.1) then
       call system("grep -o -P 'CLEAN BMAJ=.*' "//fn(1:nchar(fn))//
     /      " > "//scratch(1:nchar(scratch))//"b")
       fn2='b' 
       call getline(fn2,scratch,dumi1)
       if (dumi1.gt.0) then
        open(unit=21,file=scratch(1:nchar(scratch))//'b',status='old')
        read (21,*) dumc,dumc,bmaj,dumc,bmin,dumc,bpa
        close(21)
       end if
      end if 
       
      cmd='cp '//scratch(1:nchar(scratch))//'a '//fn(1:nchar(fn))
      call system(cmd)

      if (dumi.eq.1.and.dumi1.gt.0) then
       open(unit=21,file=fn(1:nchar(fn)),access='append')
       write (21,*) 'BMAJ = ',bmaj
       write (21,*) 'BMIN = ',bmin
       write (21,*) 'BPA = ',bpa
       close(21)
      end if 
      call system('rm -f '//scratch(1:nchar(scratch))//'a '//
     /     scratch(1:nchar(scratch))//'b')

c! now to fix for wenss images -- beam is not in header. will put 54" for all and ignore cosec(dec)
      call system("grep 'WENSS' "//fn(1:nchar(fn))//
     /         " | wc -l > "//scratch(1:nchar(scratch))//"b")
      fn40='b' 
      open(unit=24,file=scratch(1:nchar(scratch))//"b")
       read (24,*) dumi
      close(24)
      if (dumi.ge.1) then
       extn='.header'
       keyword='CDELT2'
       call get_keyword(filename,extn,keyword,keystrng,cdelt2,
     /       comment,'r',scratch)
       keyword='CRVAL2'
       call get_keyword(filename,extn,keyword,keystrng,crval2,
     /       comment,'r',scratch)
       keyword='CRPIX2'
       call get_keyword(filename,extn,keyword,keystrng,crpix2,
     /       comment,'r',scratch)
       keyword='NAXIS2'
       call get_keyword(filename,extn,keyword,keystrng,naxis2,
     /       comment,'r',scratch)
       dec=(naxis2/2.d0-crpix2)*cdelt2+crval2 
 
       open(unit=21,file=fn(1:nchar(fn)),access='append')
       write (21,*) 'BMAJ = ',54.d0/3600.d0
       write (21,*) 'BMIN = ',54.d0/3600.d0/dsin(dec/rad)
       write (21,*) 'BPA = ',0.d0
       write (21,*) 'CROTA1 = ',0.d0
       write (21,*) 'CROTA2 = ',0.d0
       write (21,*) 'CROTA3 = ',0.d0
       close(21)
      end if 

c!
      cmd="sed 's/$/ comment/g' "//fn(1:nchar(fn))//" > "//
     /    scratch(1:nchar(scratch))//"a"
      call system(cmd)
      call system('mv '//scratch(1:nchar(scratch))//'a '//
     /     fn(1:nchar(fn)))

      end

