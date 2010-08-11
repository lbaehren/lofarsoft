c! to be called form python with header info and a coord.
c! calculates wcs and then converts coord to pix

        subroutine call_wcs_c2p(ra,dec,crpix,cdelt,crval,crota,
     /             ctype1,ctype2,ctype3,x,y)
        implicit none
        include "wcs_bdsm.inc"
        include "constants.inc"
        integer wcslen,err
        parameter (wcslen=450)
        integer wcs(wcslen)
        real*8 x,y,crpix(3),cdelt(3),crval(3),crota(3),ra,dec
        character ctype(3)*8,ctype1,ctype2,ctype3

cf2py   intent(in) ra,dec,ctype,crpix,cdelt,crval,crota
cf2py   intent(out) x,y

        ctype(1)=ctype1
        ctype(2)=ctype2
        ctype(3)=ctype3
        call wcs_struct(3,ctype,crpix,cdelt,crval,crota,wcs,wcslen)

        ra=ra/rad
        dec=dec/rad
        call wcs_radec2xy(ra,dec,x,y,err,wcs,wcslen)

        return
        end



