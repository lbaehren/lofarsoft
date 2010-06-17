c! to be called form python with header info and a coord.
c! calculates wcs and then converts coord to pix

        subroutine call_wcs_p2c(x,y,crpix,cdelt,crval,crota,
     /             ctype1,ctype2,ctype3,ra,dec)
        implicit none
        include "wcs_bdsm.inc"
        include "constants.inc"
        integer wcslen,err
        parameter (wcslen=450)
        integer wcs(wcslen)
        real*8 x,y,crpix(3),cdelt(3),crval(3),crota(3),ra,dec
        character ctype(3)*8,ctype1,ctype2,ctype3

cf2py   intent(in) x,y,ctype,crpix,cdelt,crval,crota
cf2py   intent(out) ra,dec

        ctype(1)=ctype1
        ctype(2)=ctype2
        ctype(3)=ctype3
        call wcs_struct(3,ctype,crpix,cdelt,crval,crota,wcs,wcslen)

        call wcs_xy2radec(x,y,ra,dec,err,wcs,wcslen)

        ra=ra*rad
        dec=dec*rad
        
        return
        end



