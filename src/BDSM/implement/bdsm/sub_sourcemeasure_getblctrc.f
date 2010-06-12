
        subroutine sub_sourcemeasure_getblctrc(maxmem,npix_isl,xpix,
     /             ypix,bl,tr,n,m)
        implicit none
        integer maxmem,npix_isl,n,m
        integer xpix(maxmem),ypix(maxmem),bl(2),tr(2)

        call rangeint(xpix,maxmem,npix_isl,bl(1),tr(1))
        call rangeint(ypix,maxmem,npix_isl,bl(2),tr(2))
        bl(1)=max(bl(1)-1,1)
        tr(1)=min(tr(1)+1,n)
        bl(2)=max(bl(2)-1,1)
        tr(2)=min(tr(2)+1,m)

        return  
        end

