c! modified from twcs.f to convert pixel to coordinates.
c! assumes u have defined wcs struct before using wcs_struct.f

        subroutine wcs_radec2xy(ra,dec,x,y,error1,wcs,wcslen)
        implicit none
        include "constants.inc"
        integer stat,status,wcslen,wcs(wcslen),error1
        real*8  IMG(2,1),PHI,PIXEL1(2,1),PIXEL2(2,1),THETA
        real*8 WORLD1(2,1),WORLD2(2,1),x,y,ra,dec

        INCLUDE 'wcs_bdsm.inc'
        INCLUDE 'cel.inc'
        INCLUDE 'prj.inc'

        world1(1,1)=ra*rad
        world1(2,1)=dec*rad
        STATUS = WCSS2P (WCS, 1, 2, WORLD1, PHI, THETA, IMG,
     :                    PIXEL1, STAT)

        x=pixel1(1,1)
        y=pixel1(2,1)
        error1=status

        return
        END



