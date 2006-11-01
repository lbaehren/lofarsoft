
        subroutine get_fitskeywords(imsize,crval1,crval2,crpix1,crpix2)
        implicit none
        integer imsize,hh,mm,dd,ma
        real*8 crval1,crval2,crpix1,crpix2,sa,ss

        write (*,*) 
        write (*,*) '  Now to fix coordinate system on the grid'
        write (*,'(a12,i5,a3,i5,a9)') '   Image is ',imsize,' X ',
     /         imsize,' in space'
        write (*,'(a20,i5,a1,i5,a4,$)') '   Reference pixel (',imsize/2,
     /         ',',imsize/2,') : '
        read (*,*) crpix1,crpix2
        write (*,'(a49,$)') 
     /        '   RA, Dec at refpix (hh mm ss.s Sdd Sma Ssa.sa) : '
        read (*,*) hh,mm,ss,dd,ma,sa
        crval1=(hh+mm/60.d0+ss/3600.d0)*15.d0
        if (dd.lt.0.or.ma.lt.0.or.sa.lt.0.d0) then
         crval2=-1.d0*(abs(dd)+abs(ma)/60.d0+abs(sa)/3600.d0)
        else
         crval2=(dd+ma/60.d0+sa/3600.d0)
        end if

        return
        end


