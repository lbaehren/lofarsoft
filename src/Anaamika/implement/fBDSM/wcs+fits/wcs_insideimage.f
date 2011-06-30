
        subroutine wcs_insideimage(x,y,wcs,wcslen,n,m,isgoodpix)
        implicit none
        INCLUDE 'wcs_bdsm.inc'
        INCLUDE 'cel.inc'
        INCLUDE 'prj.inc'
        include 'constants.inc'
        logical isgoodpix
        integer stat,status1,wcslen,wcs(wcslen),status2,n,m
        real*8  IMG(2,1),PHI,PIXEL1(2,1),THETA
        real*8 x,y,ra,dec,x1,y1,WORLD1(2,1)

        world1(1,1)=x
        world1(2,1)=y
        STATUS1 = WCSS2P (WCS, 1, 2, WORLD1, PHI, THETA, IMG,
     /                    PIXEL1, STAT)

        isgoodpix=.true.
        if (status1.ne.0) isgoodpix=.false.
        if (pixel1(1,1).lt.1.or.pixel1(1,1).gt.max(n,m).or.
     /      pixel1(2,1).lt.1.or.pixel1(2,1).gt.max(n,m)) 
     /      isgoodpix=.false.

        return
        end

