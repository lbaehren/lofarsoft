
        subroutine wcs_isgoodcoord(x,y,wcs,wcslen,isgoodpix)
        implicit none
        INCLUDE 'wcs_bdsm.inc'
        INCLUDE 'cel.inc'
        INCLUDE 'prj.inc'
        include 'constants.inc'
        logical isgoodpix
        integer stat,status1,wcslen,wcs(wcslen),status2
        real*8  IMG(2,1),PHI,PIXEL1(2,1),world2(2,1),THETA
        real*8 x,y,ra,dec,x1,y1,WORLD1(2,1)

        world1(1,1)=x
        world1(2,1)=y
        STATUS2 = WCSS2P (WCS, 1, 2, WORLD1, PHI, THETA, IMG,
     /                    PIXEL1, STAT)
        STATUS1 = WCSP2S (WCS, 1, 2, PIXEL1, IMG, PHI, THETA,
     /                    WORLD2, STAT)

        isgoodpix=.true.
        if (status1.ne.0.or.status2.ne.0) isgoodpix=.false.
        if (abs(world1(1,1)-world2(1,1))/world1(1,1).gt.0.01d0.or.
     /      abs(world1(2,1)-world2(2,1))/world1(2,1).gt.0.01d0) 
     /      isgoodpix=.false.

        return
        end

