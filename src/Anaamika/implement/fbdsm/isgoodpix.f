c! converts pix to coords and then back and returns (if same) and coords

        subroutine isgoodpix(x,y,ra,dec,isgoodp,ctype,crpix,
     /       cdelt,crval,crota,n)
        implicit none
        logical isgoodp
        integer n,error
        character ctype(n)*8
        real*8 crpix(n),cdelt(n),crval(n),crota(n),bm_pix(n)
        real*8 x,y,ra,dec,x1,y1

        isgoodp=.false.
        call xy2radec(x,y,ra,dec,error,ctype,crpix,cdelt,crval,crota)
        call correctrarad(ra)
        call radec2xy(ra,dec,x1,y1,error,ctype,crpix,cdelt,crval,crota)
        if (abs(x1-x).lt.2.d0.and.abs(y1-y).lt.2.d0) isgoodp=.true.

        return
        end
