
        subroutine sub_readislpix(nn,npix_isl,maxmem,xpix,ypix)
        implicit none
        integer nn,npix_isl,maxmem
        integer xy(2*npix_isl),i,xpix(maxmem),ypix(maxmem)

        read (nn) xy
        do i=1,npix_isl
         xpix(i)=xy(2*i-1)
         ypix(i)=xy(2*i)
        end do

        return  
        end

