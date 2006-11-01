c! f1 is the image and f2 is the integer binary island matrix (rank converted to island).
c! latter is output of formislands.f

        subroutine iland_mat2list(f1,f2,runcode,scratch)
        implicit none
        integer n,m,nisl,maxmem,nchar
        character f1*500,f2*500,extn*10,history*10,f3*500,ch1*1
        character runcode*2,scratch*500

        if (runcode(2:2).eq.'q') write (*,*) '  Converting formats'
        extn='.img'
        call readarraysize(f1,extn,n,m)                 ! read in n,m

        extn='.nmg'
        f3=f2(1:nchar(f2))//'.rank'
        call readarraysize_bin_int(n,m,f3,extn,history)
        read (history,*) nisl,maxmem                    ! read in nisl, maxmem

        call sub_iland_mat2list(f1,f2,n,m,nisl,maxmem,runcode,scratch)

c! next step
        if (runcode(1:1).eq.'m') then
         write (*,*) 
         write (*,'(a,$)') '   Continue (fit sources : fiti) (y)/n ? '
         read (*,'(a1)') ch1
         if (ichar(ch1).eq.32) ch1='y'
333      continue
         if (ch1.eq.'y') call sourcemeasure(f1,f2,runcode,scratch)
        end if
        
        return
        end

c! ----------------------------------------------------------------------------

        subroutine sub_iland_mat2list(f1,f2,n,m,nisl,maxmem,
     /             runcode,scratch)
        implicit none
        integer n,m,nisl,maxmem,island(n,m),i,j
        integer npix_isl(nisl),xmax,ymax,nchar
        integer xpix1(maxmem),ypix1(maxmem)
        integer xpix_isl(nisl,maxmem),ypix_isl(nisl,maxmem)
        character f1*(*),f2*(*),fn*500,extn*10,history*10
        character runcode*2,scratch*500
        real*8 arr(n,m),maxv
        
c! read in image and island rank image
        extn='.img'
        call readarray_bin(n,m,arr,n,m,f1,extn)
        extn='.nmg'
        fn=f2(1:nchar(f2))//'.rank'
        call readarray_bin_int(n,m,island,n,m,fn,extn,history)

c! now process each island thro the image
        do i=1,nisl
         npix_isl(i)=0       ! num pixels in each island
        end do 

        do i=1,n
         do j=1,m
          if (island(i,j).gt.0) then
           npix_isl(island(i,j))=npix_isl(island(i,j))+1
           xpix_isl(island(i,j),npix_isl(island(i,j)))=i    ! coord(isl #, pix num)
           ypix_isl(island(i,j),npix_isl(island(i,j)))=j
          end if
         end do
        end do

        fn=scratch(1:nchar(scratch))//f2(1:nchar(f2))//'.islandlist'
        if (runcode(2:2).eq.'v') then
         write (*,*) '  Writing file '//fn(1:nchar(fn))
         write (*,*) '  Writing file '//fn(1:nchar(fn))//'.asc'
        end if
        open(unit=21,file=fn(1:nchar(fn)),form='unformatted') 
        open(unit=22,file=fn(1:nchar(fn))//'.asc',status='unknown')
         write (21) n,m,nisl,maxmem
         write (21+1,*) n,m,nisl,maxmem
         do 100 i=1,nisl
          maxv=0.d0
          do j=1,npix_isl(i)
           if (maxv.lt.arr(xpix_isl(i,j),ypix_isl(i,j))) then
            maxv=arr(xpix_isl(i,j),ypix_isl(i,j))
            xmax=xpix_isl(i,j)
            ymax=ypix_isl(i,j)
           end if
          end do
          write (21) i,npix_isl(i),xmax,ymax,maxv
          write (21+1,*) i,npix_isl(i),xmax,ymax,maxv
          do j=1,npix_isl(i)
           xpix1(j)=xpix_isl(i,j)
           ypix1(j)=ypix_isl(i,j)
          end do
          call sub_iland_rytcoords(npix_isl(i),21,xpix1,ypix1,maxmem)
100      continue
        close(21)
        close(21+1)
        
        return
        end

        subroutine sub_iland_rytcoords(npix,nn,xpix,ypix,mm)
        implicit none
        integer nn,mm,npix,xpix(mm),ypix(mm),i,xy(2*npix)

        do i=1,npix
         xy(2*i-1)=xpix(i)
         xy(2*i)=ypix(i)
        end do
        write (nn) xy
        write (nn+1,*) xy

        return
        end



