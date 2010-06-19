c! modified from twcs.f to convert pixel to coordinates.
c! assumes u have defined wcs struct before using wcs_struct.f

        subroutine wcs_xy2radec(x,y,ra,dec,error1,wcs,wcslen)
        implicit none
        include "constants.inc"
        integer stat,status,wcslen,wcs(wcslen),error1
        real*8  IMG(2,1),PHI,PIXEL1(2,1),PIXEL2(2,1),THETA
        real*8 WORLD1(2,1),WORLD2(2,1),x,y,ra,dec

        INCLUDE 'wcs_bdsm.inc'
        INCLUDE 'cel.inc'
        INCLUDE 'prj.inc'

        pixel1(1,1)=x
        pixel1(2,1)=y
        STATUS = WCSP2S (WCS, 1, 2, PIXEL1, IMG, PHI, THETA,
     /                    WORLD2, STAT)
        ra=world2(1,1)/rad
        dec=world2(2,1)/rad
        error1=status

        return
        END



