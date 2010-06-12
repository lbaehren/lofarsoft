c! edit island file by various criteria.
c! rt now only have number of pixels per island

        subroutine islandedit(f1,scratch)
        implicit none
        character f1*(*),fn*500,cmd*10,scratch*500
        integer nchar,nisl,maxmem,minpix,n,m,newisl
        
        fn=scratch(1:nchar(scratch))//f1(1:nchar(f1))//'.islandlist'
        open(unit=21,file=fn(1:nchar(fn)),form='unformatted') 
        read (21) n,m,nisl,maxmem
        close(21)

        write (*,*) '  Editing the island file ... '
        write (*,'(a27,i4)') '     number of islands   = ',nisl
        write (*,'(a27,i4)') '     max # pixels/island =  ',maxmem
        write (*,*) 
333     continue
        write (*,'(a,$)') '   Minimum number of pixels = '
        read (*,*) minpix
        if (minpix.le.0.or.minpix.gt.maxmem) goto 333

        call get_newnum_isl_minpix(f1,minpix,newisl,scratch)
        write (*,*) '  Number of islands now = ',newisl
        write (*,'(a41,$)') '   Save good,bad island files (n/y id) : '
        read (*,'(a10)') cmd
        if (cmd.ne.'n') then 
         call write_newisl_minpix(f1,newisl,minpix,maxmem,cmd,scratch)
        end if

        return
        end
        
c! ---------------------------------------------------------------

        subroutine get_newnum_isl_minpix(f1,minpix,newisl,scratch)
        implicit none
        character f1*(*),fn*500,scratch*500
        integer nchar,nisl,maxmem,minpix,n,m,newisl
        integer i,npix_isl,xmax,ymax,j
        real*8 maxv

        fn=scratch(1:nchar(scratch))//f1(1:nchar(f1))//'.islandlist'
        open(unit=21,file=fn(1:nchar(fn)),form='unformatted') 
        read (21) n,m,nisl,maxmem
        newisl=0
        do 100 i=1,nisl
         read (21) j,npix_isl,xmax,ymax,maxv
         read (21) 
         if (npix_isl.ge.minpix) newisl=newisl+1
100     continue
        close(21)

        return
        end
        

        subroutine write_newisl_minpix(f1,newisl,minpix,maxmem,
     /      cmd,scratch)
        implicit none
        character fn1*500,fn2*500,cmd*10,f1*(*),fn3*500,scratch*500
        integer nchar,nisl,maxmem,minpix,n,m,newisl
        integer i,npix_isl,xmax,ymax,j,numg,numb
        integer xpix(maxmem),ypix(maxmem)
        real*8 maxv

        fn1=scratch(1:nchar(scratch))//f1(1:nchar(f1))//'.islandlist'
        fn2=scratch(1:nchar(scratch))//f1(1:nchar(f1))//'.'//
     /      cmd(3:nchar(cmd))//'.islandlist'
        fn3=scratch(1:nchar(scratch))//f1(1:nchar(f1))//'.'//
     /      cmd(3:nchar(cmd))//'.bad.islandlist'
        write (*,*) '  Writing file '//fn2(1:nchar(fn2))
        write (*,*) '  Writing file '//fn2(1:nchar(fn2))//'.asc'
        write (*,*) '  Writing file '//fn3(1:nchar(fn3))
        write (*,*) '  Writing file '//fn3(1:nchar(fn3))//'.asc'
        open(unit=21,file=fn1(1:nchar(fn1)),form='unformatted') 
        open(unit=22,file=fn2(1:nchar(fn2)),form='unformatted') 
        open(unit=23,file=fn2(1:nchar(fn2))//'.asc',status='unknown')
        open(unit=24,file=fn3(1:nchar(fn3)),form='unformatted') 
        open(unit=25,file=fn3(1:nchar(fn3))//'.asc',status='unknown')
         numg=0
         numb=0
         read (21) n,m,nisl,maxmem
         write (22) n,m,newisl,maxmem
         write (23,*) n,m,newisl,maxmem
         write (24) n,m,nisl-newisl,minpix-1
         write (25,*) n,m,nisl-newisl,minpix-1
         do 100 i=1,nisl
          read (21) j,npix_isl,xmax,ymax,maxv
          if (npix_isl.ge.minpix) then
           numg=numg+1
           write (22) numg,npix_isl,xmax,ymax,maxv
           write (23,*) numg,npix_isl,xmax,ymax,maxv
           call sub_readxypix(21,22,23,npix_isl)
          else
           numb=numb+1
           write (24) numb,npix_isl,xmax,ymax,maxv
           write (25,*) numb,npix_isl,xmax,ymax,maxv
           call sub_readxypix(21,24,25,npix_isl)
          end if
100      continue
        close(21)
        close(22)
        close(23)

        return
        end
        
        subroutine sub_readxypix(nn1,nn2,nn3,npix_isl)
        implicit none
        integer nn1,nn2,nn3,npix_isl
        integer xy(2*npix_isl)

        read (nn1) xy
        write (nn2) xy
        write (nn3,*) xy

        return
        end

