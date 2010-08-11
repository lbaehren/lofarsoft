
        subroutine wcs_isgoodpix(x,y,wcs,wcslen,isgoodpix)
        implicit none
        INCLUDE 'wcs_bdsm.inc'
        INCLUDE 'cel.inc'
        INCLUDE 'prj.inc'
        logical isgoodpix
        integer stat,status1,wcslen,wcs(wcslen),status2
        real*8  IMG(2,1),PHI,PIXEL1(2,1),PIXEL2(2,1),THETA
        real*8 x,y,ra,dec,x1,y1,WORLD1(2,1)

        pixel1(1,1)=x
        pixel1(2,1)=y
        STATUS1 = WCSP2S (WCS, 1, 2, PIXEL1, IMG, PHI, THETA,
     /                    WORLD1, STAT)
        STATUS2 = WCSS2P (WCS, 1, 2, WORLD1, PHI, THETA, IMG,
     /                    PIXEL2, STAT)

        isgoodpix=.true.
        if (status1.ne.0.or.status2.ne.0) isgoodpix=.false.
        if (abs(pixel1(1,1)-pixel2(1,1)).gt.3.d0.or.
     /      abs(pixel1(2,1)-pixel2(2,1)).gt.3.d0) isgoodpix=.false.

        return
        end

