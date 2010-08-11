
c! to initialise
        subroutine read_dum_head(n,ctype,crpix,cdelt,crval,crota,bm_pix)
        implicit none
        integer n
        real*8 crpix(n),cdelt(n),crval(n),crota(n),bm_pix(3)
        character ctype(n)*8

        ctype(1)='RA---SIN'
        ctype(2)='DEC--SIN'
        ctype(3)='FREQ'
        crpix(1)=1.d0
        crpix(2)=1.d0
        crpix(3)=1.d0
        cdelt(1)=1.d0/3600.d0  ! deg
        cdelt(2)=1.d0/3600.d0  ! deg
        cdelt(3)=4.d6
        crval(1)=1.5d0*15.d0   ! deg
        crval(2)=10.5d0        ! deg
        crval(3)=120.d6
        crota(1)=0.d0
        crota(2)=0.d0
        crota(3)=0.d0
        bm_pix(1)=4.d0
        bm_pix(2)=4.d0
        bm_pix(3)=0.d0

        return
        end

