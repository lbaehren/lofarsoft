c!
        subroutine writefitshead(fn,n,ctype,crpix,cdelt,crval,crota
     /             ,bm_pix,scratch)
        implicit none
        integer n,nchar
        real*8 crpix(n),cdelt(n),crval(n),crota(n),bm_pix(3)
        character ctype(n)*8,fn*500,ff*500,scratch*500

        ff=scratch(1:nchar(scratch))//fn(1:nchar(fn))//'.header'
        open(unit=21,file=ff,status='unknown')
        write (21,*) 'CTYPE1 = ',ctype(1),' comment'
        write (21,*) 'CTYPE2 = ',ctype(2),' comment'
        write (21,*) 'CTYPE3 = ',ctype(3),' comment'
        write (21,*) 'CRPIX1 = ',crpix(1),' comment'
        write (21,*) 'CRPIX2 = ',crpix(2),' comment'
        write (21,*) 'CRPIX3 = ',crpix(3),' comment'
        write (21,*) 'CDELT1 = ',cdelt(1),' comment'
        write (21,*) 'CDELT2 = ',cdelt(2),' comment'
        write (21,*) 'CDELT3 = ',cdelt(3),' comment'
        write (21,*) 'CRVAL1 = ',crval(1),' comment'
        write (21,*) 'CRVAL2 = ',crval(2),' comment'
        write (21,*) 'CRVAL3 = ',crval(3),' comment'
        write (21,*) 'CROTA1 = ',crota(1),' comment'
        write (21,*) 'CROTA1 = ',crota(2),' comment'
        write (21,*) 'CROTA3 = ',crota(3),' comment'
        write (21,*) 'BMAJ = ',bm_pix(1)*cdelt(1),' comment'
        write (21,*) 'BMIN = ',bm_pix(2)*cdelt(2),' comment'
        write (21,*) 'BPA = ',bm_pix(3),' comment'
        close(21)

        return
        end

