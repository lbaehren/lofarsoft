
        subroutine call_pix2coord(headfile,ra,dec,x,y,scratch)
        implicit none
        include "wcs_bdsm.inc"
        include "constants.inc"
        integer wcslen
        real*8 x,y,ra,dec
        character headfile*500
        real*8 crpix(3),cdelt(3),crval(3),crota(3),bmaj,bmin,bpa
        character ctype(3)*8,extn*20,scratch*500,headerfile*500
        parameter (wcslen=450)
        integer wcs(wcslen),error1

        extn=".header"
        call readheader4fits(headfile,extn,ctype,crpix,cdelt,crval,
     /         crota,bmaj,bmin,bpa,3,scratch)
        call wcs_struct(3,ctype,crpix,cdelt,crval,crota,wcs,wcslen)
        call wcs_radec2xy(ra/rad,dec/rad,x,y,error1,wcs,wcslen)

        return
        end





