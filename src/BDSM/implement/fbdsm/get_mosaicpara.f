c! get mosaic parameters and write out a file with centre pixels and size

        subroutine get_mosaicpara(fn,imsize,scratch)
        implicit none
        integer nmosx,nmosy,imsize,nchar,npointx,npointy
        real*8 overlap
        character fn*500,f1*500,scratch*500

        write (*,*) '  Mosaicing parameters now ... '
        write (*,'(a29,$)') '   No. of facets in RA,Dec : '
        read (*,*) nmosx,nmosy
        write (*,'(a19,$)') '   Overlap pixels : '
        read (*,*) overlap
        write (*,'(a32,$)') '   No. of pointings in RA,Dec : '
        read (*,*) npointx,npointy

        f1=scratch(1:nchar(scratch))//fn(1:nchar(fn))//'.mos'
        open(unit=21,file=f1,status='unknown')
        call sub_get_mosaicpara(21,imsize,npointx,npointy,0.d0)
        call sub_get_mosaicpara(21,imsize,nmosx,nmosy,overlap)
        close(21)

        return
        end


        subroutine sub_get_mosaicpara(nn,imsize,ptx,pty,olap)
        implicit none
        integer ptx,pty,subx,suby,imsize,round,nchar,nn
        integer xlow,ylow,i,j,subxl,subyl,sx,sy 
        real*8 olap

        subx=round((imsize+(ptx-1)*olap)/ptx)
        suby=round((imsize+(pty-1)*olap)/pty)
        if (ptx.ne.1) subxl=imsize-(ptx-1)*subx+(ptx-1)*olap
        if (pty.ne.1) subyl=imsize-(pty-1)*suby+(pty-1)*olap

        write (nn,*) ptx,pty
        do i=1,ptx
         do j=1,pty
          if (i.eq.ptx.and.ptx.ne.1) then
           sx=subxl
          else
           sx=subx
          end if
          if (j.eq.pty.and.pty.ne.1) then
           sy=subyl
          else
           sy=suby
          end if

          if (i.ne.ptx) then
           xlow=(i-1)*sx-(i-1)*olap+1
          else
           xlow=imsize-sx+1
          end if
          if (j.ne.pty) then
           ylow=(j-1)*sy-(j-1)*olap+1
          else
           ylow=imsize-sy+1
          end if
          write (nn,*) xlow,ylow,xlow+sx-1,ylow+sy-1 

         end do
        end do

        return
        end

